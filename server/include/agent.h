#ifndef __ENVIRO_AGENT__H
#define __ENVIRO_AGENT__H 

#include <iostream>
#include <chrono>
#include "json/json.h"
#include "elma.h"
#include "chipmunk.h"
#include "world.h"

#define AGENT_CREATE_TYPE (Agent* (*)(std::string, World&))
#define AGENT_DESTROY_TYPE (void (*)(Agent*))

#define DECLARE_INTERFACE(__CLASS_NAME__)                                         \
extern "C" __CLASS_NAME__* create_agent(std::string name, enviro::World& world) { \
    return new __CLASS_NAME__(name, world);                                       \
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
        Agent(std::string name, World& world);
        ~Agent();

        void init() {}
        void start() {}
        void update() {}
        void stop() {}

        virtual void initialize() = 0;
        virtual void step() = 0;

        Agent& apply_force(int index, cpFloat magnitude);

        json serialize();

        inline void set_destroyer(void (*f)(Agent*)) { _destroyer = f; }

        private:
        cpBody * _body;
        cpShape * _shape;
        void (* _destroyer)(Agent*);

    };    

    // Factory method
    Agent * CreateAgent(json specification, World& world);

}

#endif