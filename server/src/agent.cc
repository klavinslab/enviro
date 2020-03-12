#include <dlfcn.h>
#include <math.h>
#include "enviro.h"

#define IDENTITY { a: 1, b: 0, c: 0, d: 1, tx: 0, ty: 0 }

namespace enviro {

    static int _next_id = 0;

    Agent::Agent(json specification, World& world) : 
        _specification(specification),
        _world_ptr(&world), 
        _alive(true),
        Process(specification["definition"]["name"].get<string>()) {

        cpSpace * space = world.get_space();
        if ( !space ) {
            throw std::runtime_error("Uninitialized physics space in agent constructor");
        }

        if ( cpSpaceIsLocked(space) ) {
            throw std::runtime_error("Cannot add shapes and bodies to space when it is updating");
        }

        _id = _next_id++;

        std::cout << specification << "\n";

        if ( specification["definition"]["shape"].is_array() ) {

            int num_vertices = specification["definition"]["shape"].size();
            cpVect *vertices = (cpVect *) calloc(num_vertices, sizeof(cpVect));
            int i = 0;

            for (auto v : specification["definition"]["shape"]) {
                vertices[i++] = cpv(v["x"], v["y"]);
            }

            _moment_of_inertia = cpMomentForPoly(
                specification["definition"]["mass"], 
                num_vertices, 
                vertices, 
                cpvzero, 1);

            _body = cpSpaceAddBody(
                space, 
                cpBodyNew(
                    specification["definition"]["mass"], 
                    _moment_of_inertia));

            // std::cout << "pos func " << (long int) _body->position_func << "\n";

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
            
            std::cout << "Added new physics stuff for agent " << _id << "\n";

        } else {

            _moment_of_inertia = cpMomentForCircle(
                specification["definition"]["mass"], 
                specification["definition"]["radius"],
                specification["definition"]["radius"],
                cpv(0,0));     

            _body = cpSpaceAddBody(
                space, 
                cpBodyNew(
                    specification["definition"]["mass"], 
                    _moment_of_inertia));    

            cpBodySetPosition(_body, cpv(
                specification["position"]["x"], 
                specification["position"]["y"]
            ));     

            _shape = cpSpaceAddShape(
                space, 
                cpCircleShapeNew(_body, specification["definition"]["radius"], cpv(0,0)));  

            std::cout << "Added new physics stuff for agent " << _id << "\n";                
                                                                          
        }

        cpShapeSetFriction(_shape, specification["definition"]["friction"]["collision"].get<cpFloat>()); 
        cpShapeSetElasticity(_shape, 0.0);        

        if ( specification["definition"]["type"] == "static" ) {
            cpBodySetType(_body, CP_BODY_TYPE_STATIC);
        }

        cpShapeSetCollisionType(_shape, AGENT_COLLISION_TYPE);
        cpBodySetUserData(_body, this);

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
            return _sensors[index]->value().first;
        } else {
            throw Exception("Sensor index out of range");
        }
    }    

    std::string Agent::sensor_reflection_type(int index) {
        if ( index < _sensors.size() ) {
            return _sensors[index]->value().second;
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

    std::vector<std::string> Agent::sensor_reflection_types() {
        std::vector<std::string> values;
        for ( int i=0; i<_sensors.size(); i++ ) {
            values.push_back(sensor_reflection_type(i));
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

    Agent& Agent::omni_apply_force(cpFloat fx, cpFloat fy) {

        if ( is_static() ) {
            return *this;
        }

        cpFloat kL = linear_friction(); 
        cpFloat kR = rotational_friction(); 

        cpVect f = { 
            x: fx,
            y: fy
        };

        cpVect F = cpvadd(cpvmult(velocity(), -kL), f);

        cpBodySetForce(_body, F );
        cpBodySetTorque(_body, - kR * angular_velocity() );

        return *this;

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

    Agent& Agent::omni_track_velocity(double vx, double vy, double k) {
        return omni_apply_force(
            - k * ( velocity().x - vx),
            - k * ( velocity().y - vy)
        );        
    }

    Agent& Agent::damp_movement() {
        return track_velocity(0,0);
    }

    Agent& Agent::omni_damp_movement() {
        return omni_apply_force(0,0);
    }    

    double normalize_angle(double angle) {
        // Returns a in the range -pi, pi
        double a = angle;
        while ( a > M_PI ) {
            a -= 2*M_PI;
        }
        while ( a < -M_PI ) {
            a += 2*M_PI;
        }
        return a;
    }

    Agent& Agent::move_toward(cpFloat x, cpFloat y, double vF, double vR) {

        cpVect p = position();
        double f = 0,
               theta_error = normalize_angle(angle() - atan2(y - p.y, x - p.x)),
               d = cpvdist(p,cpv(x,y)),
               t = sin(theta_error);

        f = d < 10 ? 0.1*d : 1;
        f = f * exp(-4*theta_error*theta_error);
        t = theta_error;
        apply_force(vF*f, -vR*t);

        return *this;
    }

    Agent& Agent::omni_move_toward(cpFloat x, cpFloat y, double v) {

        cpVect p = position();
        double d = cpvdist(p,cpv(x,y));

        cpVect V = cpvmult(
            cpvsub(cpv(x,y),p),
            v * d / ( 5 + d )
        );

        omni_track_velocity(V.x, V.y);

        return *this;
    }    

    Agent& Agent::teleport(cpFloat x, cpFloat y, cpFloat theta) {
        cpBodySetPosition(_body, {x: x, y: y});
        cpBodySetAngle(_body, theta);
        cpBodySetVelocity(_body, {x:0, y:0});
        cpBodySetAngularVelocity(_body,0);
        return *this;
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
        std::cout << "Deleting agent " << get_id() << "\n";
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
            {"sensors", sensor_values() },
            {"decoration", _decoration },
            {"label", {
                    { "text", _label },
                    { "x", _label_x },
                    { "y", _label_y }
                } 
            }
        };            
    }

    // Collisions
    Agent& Agent::notice_collisions_with(const std::string agent_type, std::function<void(Event&)> handler) {
        collision_handlers[agent_type] = handler;
        return *this;
    }

    Agent& Agent::ignore_collisions_with(const std::string agent_type) {
        collision_handlers.erase(agent_type);
        return *this;
    }  

    Agent& Agent::handle_collision(const Agent &other) {
        std::string other_type = other.definition()["name"];
        auto i = collision_handlers.find(other_type);
        if ( i != collision_handlers.end() ) {
            auto handler = collision_handlers[other_type];
            Event e("collision", {
                { "type", other_type },
                { "x", other.x() },
                { "y", other.y() },
                {"id", other.get_id() }
            });
            handler(e);
        }
        return *this;
    }

    // Constraints
    Agent& Agent::find_agent(int id) {
        return _world_ptr->find_agent(id);
    }

    Agent& Agent::attach_to(Agent &agent) {
        _world_ptr->add_constraint(*this, agent);
        return *this;
    } 

    Agent& Agent::prevent_rotation() {
        cpBodySetMoment(_body, INFINITY);
        return *this;
    }

    Agent& Agent::allow_rotation() {
        cpBodySetMoment(_body, _moment_of_inertia);
        return *this;
    }     

    // Styles
    Agent& Agent::set_style(json style) {
        _specification["style"] = style;
        return *this;
    }

    // Agent Management
    void Agent::remove_agent(int id) { _world_ptr->remove(id); }

    bool Agent::agent_exists(int id) { return _world_ptr->exists(id); }

    Agent& Agent::add_agent(const std::string name, double x, double y, double theta, const json style) { 
        return _world_ptr->add_agent(name,x,y,theta,style); 
    }

    // Class wide methods /////////////////////////////////////////

    json Agent::build_specification(json agent_entry) {

        json result = agent_entry, definition;
        try {
            definition = json_helper::read(result["definition"]);
        } catch ( const nlohmann::detail::parse_error &e ) {
            std::string msg = "Could not parse ";
            msg += result["definition"];
            msg += ": ";
            msg += e.what();
            throw std::runtime_error(msg);
        }

        if ( !definition["shape"].is_null() && definition["shape"].is_array() ) {
            json_helper::check(definition, ENVIRO_AGENT_SCHEMA);
        } else if ( !definition["shape"].is_null() && definition["shape"].is_string() && definition["shape"] == "omni" ) {
            json_helper::check(definition, ENVIRO_OMNI_AGENT_SCHEMA);
        } else {
            std::string msg = "Could not find a valid shape definition in agent definition in ";
            msg += result["definition"];
            throw std::runtime_error(msg);
        }

        result["definition"] = definition;
        return result;

    }

}
