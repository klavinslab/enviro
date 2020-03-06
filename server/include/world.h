#ifndef __ENVIRO_WORLD__H
#define __ENVIRO_WORLD__H 

#include <iostream>
#include <chrono>
#include "elma/elma.h"
#include "chipmunk.h"
#include "enviro.h"

using namespace std::chrono;
using namespace elma;

namespace enviro {

    class Agent;

    class World : public Process {
        public:

        World(json config);
        ~World();

        void init() {}
        void start() {}
        void update();
        void stop() {}

        inline cpSpace * get_space() { return space; }
        World& add_agent(Agent& agent);
        World& all(std::function<void(Agent&)> f);
        inline json get_config() const { return config; }

        private:
        vector<Agent *> agents;
        cpSpace * space;
        cpFloat timeStep;
        json config;

    };

}

#endif