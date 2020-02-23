#ifndef __ENVIRO_AGENT__H
#define __ENVIRO_AGENT__H 

#include <iostream>
#include <chrono>
#include "elma/elma.h"
#include "chipmunk.h"
#include "enviro.h"

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
        ~Agent();

        void init();
        void start();
        void update();
        void stop();

        Agent& apply_force(int index, cpFloat magnitude);
        json serialize();
        inline void set_destroyer(void (*f)(Agent*)) { _destroyer = f; }
        inline int get_id() { return _id; }

        //! Apply the given thrust and torque.
        void actuate(cpFloat thrust, cpFloat torque);

        //! Attempt to track a given linear and angular velocity.
        void servo(cpFloat linear_velocity, cpFloat angular_velocity);

        //! Slow the agent down
        void damp_movement();

        inline double linear_friction() const {
            return _specification["definition"]["friction"]["linear"];
        }

        inline double rotational_friction() const {
            return _specification["definition"]["friction"]["rotational"];
        } 

        Agent& add_process(Process &p);

        private:
        cpBody * _body;
        cpShape * _shape;
        void (* _destroyer)(Agent*);
        int _id;
        json _specification;
        std::vector<Process *> _processes;

        public:
        // Static methods

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