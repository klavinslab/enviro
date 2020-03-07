#ifndef __ENVIRO_AGENT__H
#define __ENVIRO_AGENT__H 

#include <iostream>
#include <chrono>
#include "elma/elma.h"
#include "chipmunk.h"
#include "enviro.h"
#include "sensor.h"

#define DBG std::cout << __FILE__ << ":" << __LINE__ << "\n";

#define AGENT_CREATE_TYPE (Agent* (*)(json spec, World&))
#define AGENT_DESTROY_TYPE (void (*)(Agent*))

#define DECLARE_INTERFACE(__CLASS_NAME__)                                         \
extern "C" __CLASS_NAME__* create_agent(json spec, enviro::World& world) {        \
    return new __CLASS_NAME__(spec, world);                                       \
}                                                                                 \
extern "C" void destroy_agent( __CLASS_NAME__* object ) {                         \
    delete object;                                                                \
}

using namespace std::chrono;
using namespace elma;
using nlohmann::json; 

namespace enviro {

    class World;
    class Controller;
    class AgentInterface;

    class Agent : public Process {

        friend class World;

        public:

        Agent(json specification, World& world);
        Agent& add_process(Process &p);
        Agent& add_process(StateMachine &m);
        json serialize();
        inline void set_destroyer(void (*f)(Agent*)) { _destroyer = f; }     
        ~Agent();

        // Elma Process methods
        void init();
        void start();
        void update();
        void stop();

        // State getters
        inline cpVect position() const { return cpBodyGetPosition(_body); }
        inline double x() const { return cpBodyGetPosition(_body).x; }
        inline double y() const { return cpBodyGetPosition(_body).y; }

        inline cpVect velocity() const { return cpBodyGetVelocity(_body); }
        inline double vx() const { return cpBodyGetVelocity(_body).x; }
        inline double vy() const { return cpBodyGetVelocity(_body).y; }

        inline double angle() const { return cpBodyGetAngle(_body); }
        inline double angular_velocity() const { return cpBodyGetAngularVelocity(_body); }

        // Actuators
        Agent& omni_apply_force(cpFloat fx, cpFloat fy);
        Agent& apply_force(cpFloat thrust, cpFloat torque);
        Agent& track_velocity(cpFloat linear_velocity, cpFloat angular_velocity, cpFloat kL=10, cpFloat kR=10);
        Agent& omni_track_velocity(double vx, double vy, double k=10);  
        Agent& damp_movement();
        Agent& omni_damp_movement();
        Agent& move_toward(cpFloat x, cpFloat y, double vF=75, double vR=20);
        Agent& omni_move_toward(double x, double y, double v=1);
        Agent& teleport(cpFloat x, cpFloat y, cpFloat theta);

        // Parameter getters
        inline json definition() const { return  _specification["definition"]; }
        inline json friction() const { return definition()["friction"]; }
        inline double linear_friction() const { return friction()["linear"].get<cpFloat>(); }
        inline double rotational_friction() const { return friction()["rotational"].get<cpFloat>(); }
        inline bool is_static() const { return _specification["definition"]["type"] == "static"; }

        // Sensor methods
        double sensor_value(int index);
        std::vector<double> sensor_values();

        // Other getters
        inline World * get_world_ptr() { return _world_ptr; }
        inline cpShape * get_shape() { return _shape; } 
        inline int get_id() { return _id; }  

        private:
        cpBody * _body;
        cpShape * _shape;
        void (* _destroyer)(Agent*);
        int _id;
        json _specification;
        std::vector<Process *> _processes;
        std::vector<Sensor *> _sensors;
        World * _world_ptr;
        void setup_sensors();

        // Static methods

        public:
        
        //! This method creates a new agent using the json specification and the
        //! DLL it points to. This can't be a constructor because it actually
        //! builds a class that derives from Agent, but is not the Agent class
        //! itself.
        static Agent * create_from_specification(json specification, World& world);

        //! This method takes an agent entry in the config.json file
        //! and replaces its "definition" field with the definition json
        //! in the defs directory.
        static json build_specification(json agent_entry);

    };

}

#endif