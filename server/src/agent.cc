#include <dlfcn.h>
#include <math.h>
#include "agent.h"

#define IDENTITY { a: 1, b: 0, c: 0, d: 1, tx: 0, ty: 0 }

namespace enviro {

    static int _next_id = 0;

    Agent::Agent(json specification, World& world) : _specification(specification), Process() {

        cpSpace * space = world.get_space();

        std::cout << specification << "\n";

        int num_vertices = specification["definition"]["shape"].size();
        cpVect *vertices = (cpVect *) calloc(num_vertices, sizeof(cpVect));
        int i = 0;

        for (auto v : specification["definition"]["shape"]) {
            vertices[i++] = cpv(v[0], v[1]);
            std::cout << v << "\n";
        }

        cpFloat moment = cpMomentForPoly(
            specification["definition"]["mass"], 
            num_vertices, 
            vertices, 
            cpvzero, 1);

        _body = cpSpaceAddBody(
            space, 
            cpBodyNew(
                specification["definition"]["mass"], 
                moment));

        cpBodySetPosition(_body, cpv(
            specification["position"]["x"], 
            specification["position"]["y"]
        ));

        cpBodySetAngle(_body, specification["position"]["theta"]);

        _shape = cpSpaceAddShape(
            space, 
            cpPolyShapeNew(
                _body, 
                num_vertices, 
                vertices, 
                IDENTITY, 1));

        cpShapeSetFriction(_shape, specification["definition"]["friction"]["collision"].get<cpFloat>());

        if ( specification["definition"]["type"] == "static" ) {
            cpBodySetType(_body, CP_BODY_TYPE_STATIC);
        }

        // TODO: DELETE VERTICES? Or make the instance vars and delete with destructor?
        _id = _next_id++;

    }

    void Agent::actuate(cpFloat thrust, cpFloat torque) {

        if ( _specification["definition"]["type"] == "static" ) {
            return;
        }

        cpVect v      = cpBodyGetVelocity(_body);
        cpFloat theta = cpBodyGetAngle(_body);
        cpFloat omega = cpBodyGetAngularVelocity(_body);

        cpFloat kL = _specification["definition"]["friction"]["linear"].get<cpFloat>();  
        cpFloat kR = _specification["definition"]["friction"]["rotational"].get<cpFloat>();

        if (!r) {
            r = true;
            std::cout << kL << ", " << kR << "\n";
        }

        cpVect f = { 
            x: thrust * cos(theta), 
            y: thrust * sin(theta)
        } ;

        cpVect F = cpvadd(cpvmult(v, -kL), f);

        cpBodySetForce(_body, F );
        cpBodySetTorque(_body, torque - kR * omega );

    }

    void Agent::servo(cpFloat linear_velocity, cpFloat angular_velocity) {

        cpVect v  = cpBodyGetVelocity(_body);
        cpFloat theta = cpBodyGetAngle(_body);
        cpFloat V = v.x * cos(theta);        // this should be the x component of the 
                                             // current velocity vector projected
                                             // along the vector [1,0].
        cpFloat omega = cpBodyGetAngularVelocity(_body);

        actuate(
            10*(linear_velocity - V), 
            1000*(angular_velocity-omega)
        );

    }

    Agent::~Agent() {
        cpShapeFree(_shape);
        cpBodyFree(_body);
    }    

    json Agent::serialize() {
        cpVect pos = cpBodyGetPosition(_body);
        cpVect vel = cpBodyGetVelocity(_body);
        return { 
            {"id", get_id()}, 
            {"position", { 
                    { "x", pos.x},
                    { "y", pos.y},
                    { "theta", cpBodyGetAngle(_body)}
                },
            },
            {"velocity", { 
                    { "x", vel.x},
                    { "y", vel.y},
                    { "theta", cpBodyGetAngularVelocity(_body)}
                },
            },
            {"specification", _specification}
        };            
    }

    // Class wide methods /////////////////////////////////////////

    json Agent::build_specification(json agent_entry) {

        json result = agent_entry, definition;

        std::ifstream ifs(result["definition"]);
        if ( ifs.fail() ) {
            std:string msg = "Could not open agent definition file ";
            msg += result["definition"];
            throw std::runtime_error(msg);
        }

        ifs >> definition;
        result["definition"] = definition;
        return result;
    }  

    Agent * Agent::create_from_specification(json spec, World& world) {

        auto file = spec["definition"]["controller"].get<std::string>().c_str();
        auto handle = dlopen(file , RTLD_LAZY);
        if (!handle) {
            throw std::runtime_error(dlerror());
        }
        auto create_agent = AGENT_CREATE_TYPE dlsym(handle, "create_agent");
        auto agent_ptr = (Agent*) create_agent(spec, world); 
        agent_ptr->set_destroyer(AGENT_DESTROY_TYPE dlsym(handle, "destroy_agent"));
        return agent_ptr;

    }

}