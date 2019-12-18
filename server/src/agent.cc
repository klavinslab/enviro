#include <dlfcn.h>
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
        DBG
        for (auto v : specification["definition"]["shape"]) {
            vertices[i++] = cpv(v[0], v[1]);
            std::cout << v << "\n";
        }

        DBG

        cpFloat moment = cpMomentForPoly(
            specification["definition"]["mass"], 
            num_vertices, 
            vertices, 
            cpvzero, 1);

        DBG
                 
        _body = cpSpaceAddBody(space, cpBodyNew(specification["definition"]["mass"], moment));

        DBG

        cpBodySetPosition(_body, cpv(
            specification["position"]["x"], 
            specification["position"]["y"]
        ));

        cpBodySetAngle(_body, specification["position"]["theta"]);

        DBG

        _shape = cpSpaceAddShape(
            space, 
            cpPolyShapeNew(
                _body, 
                num_vertices, 
                vertices, 
                IDENTITY, 1));

        cpShapeSetFriction(_shape, specification["definition"]["friction"]);

        DBG

        // TODO: DELETE VERTICES??

        _id = _next_id++;

    }

    Agent::~Agent() {
        cpShapeFree(_shape);
        cpBodyFree(_body);
    }    

    json Agent::serialize() {
        cpVect pos = cpBodyGetPosition(_body);
        cpVect vel = cpBodyGetVelocity(_body);
        DBG
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