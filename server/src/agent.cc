#include <dlfcn.h>
#include <math.h>
#include "enviro.h"

#define IDENTITY { a: 1, b: 0, c: 0, d: 1, tx: 0, ty: 0 }

namespace enviro {

    static int _next_id = 0;

    Agent::Agent(json specification, World& world) : 
        _specification(specification), 
        _world_ptr(&world), 
        Process(specification["definition"]["name"].get<string>()) {

        cpSpace * space = world.get_space();

        int num_vertices = specification["definition"]["shape"].size();
        cpVect *vertices = (cpVect *) calloc(num_vertices, sizeof(cpVect));
        int i = 0;

        for (auto v : specification["definition"]["shape"]) {
            vertices[i++] = cpv(v["x"], v["y"]);
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

        // TODO: DELETE VERTICES? Or make them instance vars and delete with destructor?
        _id = _next_id++;

        setup_sensors();

    }

    void Agent::setup_sensors() {
        for ( auto spec : _specification["definition"]["sensors"] ) {
            if ( spec["type"] == "range" ) {
                _sensors.push_back(new RangeSensor(
                    *this,
                    spec["location"]["x"],
                    spec["location"]["y"],
                    spec["direction"]
                ));
            } else {
                throw Exception("Unknown Sensor Type (only 'range' is supported at this time");
            }
        }
    }

    double Agent::sensor_value(int index) {
        if ( index < _sensors.size() ) {
            return _sensors[index]->value();
        } else {
            throw Exception("Sensor index out of range");
        }
    }    

    std::vector<double> Agent::sensor_values() {
        std::vector<double> values;
        for ( int i=0; i<_sensors.size(); i++ ) {
            values.push_back(sensor_value(i));
        }
        return values;
    }

    void Agent::init() {
        for ( Process * p : _processes ) {
            p->set_manager(_manager_ptr);
            p->init();
        }
    }

    void Agent::start() {
        for ( Process * p : _processes ) {
            p->start();
        }
    }

    void Agent::update() {
        for ( Process * p : _processes ) {
            p->update();
        }
    }

    void Agent::stop() {
        for ( Process * p : _processes ) {
            p->stop();
        }
    }

    Agent& Agent::apply_force(cpFloat thrust, cpFloat torque) {

        if ( is_static() ) {
            return *this;
        }

        cpFloat kL = linear_friction();  
        cpFloat kR = rotational_friction();

        cpVect f = { 
            x: thrust * cos(angle()),
            y: thrust * sin(angle())
        } ;

        cpVect F = cpvadd(cpvmult(velocity(), -kL), f);

        cpBodySetForce(_body, F );
        cpBodySetTorque(_body, torque - kR * angular_velocity() );

        return *this;

    }

    Agent& Agent::track_velocity(cpFloat linear_velocity_target, cpFloat angular_velocity_target, 
                                 cpFloat kL, cpFloat kR) {

        cpFloat V = velocity().x * cos(angle());  // this is the x component of the 
                                                  // current velocity vector projected
                                                  // along the vector [1,0].
        return apply_force(
            kL*(linear_velocity_target - V), 
            kR*(angular_velocity_target-angular_velocity())
        );

    }

    Agent& Agent::damp_movement() {
        return track_velocity(0,0);
    }

    Agent& Agent::add_process(Process &p) {
        _processes.push_back(&p);
        AgentInterface * ai = dynamic_cast<AgentInterface *>(&p);
        ai->use_agent(*this);
        return *this;
    }

    Agent& Agent::add_process(StateMachine &m) {

        add_process(static_cast<Process&>(m));

        for ( Transition& t : m.transitions() ) {
            AgentInterface * ai = dynamic_cast<AgentInterface *>(&t.from());
            ai->use_agent(*this);
            ai = dynamic_cast<AgentInterface *>(&t.to());
            ai->use_agent(*this);
        }

        return *this;
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
            {"specification", _specification},
            {"sensors", sensor_values() }
        };            
    }

    // Class wide methods /////////////////////////////////////////

    json Agent::build_specification(json agent_entry) {

        json result = agent_entry, definition;
        definition = json_helper::read(result["definition"]);
        json_helper::check(definition, ENVIRO_AGENT_SCHEMA);

        result["definition"] = definition;
        return result;
    }  

    Agent * Agent::create_from_specification(json spec, World& world) {

        auto file = spec["definition"]["controller"].get<std::string>();
        auto handle = dlopen(file.c_str() , RTLD_LAZY);
        if (!handle) {
            std::cerr << "Error: " << file << "\n";
            throw std::runtime_error(dlerror());
        }
        auto create_agent = AGENT_CREATE_TYPE dlsym(handle, "create_agent");
        auto agent_ptr = (Agent*) create_agent(spec, world); 
        agent_ptr->set_destroyer(AGENT_DESTROY_TYPE dlsym(handle, "destroy_agent"));
        return agent_ptr;

    }

}
