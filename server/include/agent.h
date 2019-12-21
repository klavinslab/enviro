#ifndef __ENVIRO_AGENT__H
#define __ENVIRO_AGENT__H 

#include <iostream>
#include <chrono>
#include "json/json.h"
#include "elma.h"
#include "chipmunk.h"
#include "world.h"

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

    class Agent : public Process {

        friend class World;

        public:
        Agent(json specification, World& world);
        ~Agent();

        Agent& apply_force(int index, cpFloat magnitude);
        json serialize();
        inline void set_destroyer(void (*f)(Agent*)) { _destroyer = f; }
        inline int get_id() { return _id; }

        void actuate(cpFloat thrust, cpFloat torque);
        void servo(cpFloat linear_velocity, cpFloat angular_velocity);

        private:
        cpBody * _body;
        cpShape * _shape;
        void (* _destroyer)(Agent*);
        int _id;
        json _specification;

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

        bool r = false;

    };



}

#endif