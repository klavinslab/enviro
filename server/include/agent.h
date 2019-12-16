#ifndef __ENVIRO_AGENT__H
#define __ENVIRO_AGENT__H 

#include <iostream>
#include <chrono>
#include "json/json.h"
#include "elma.h"
#include "chipmunk.h"
#include "world.h"

using namespace std::chrono;
using namespace elma;
using nlohmann::json; 

namespace enviro {

    class World;

    class Agent : public Process {

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
        cpBody *body;
        cpShape *shape;

    };

}

#endif