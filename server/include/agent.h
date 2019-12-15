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
        json serialize();
        cpBody *body;
        cpShape *shape;

    };

}

#endif