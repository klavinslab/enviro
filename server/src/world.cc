#include "world.h"

namespace enviro {

    World::World(std::string name) : Process(name) {
        space = cpSpaceNew();
        timeStep = 1.0/60.0;
    }

    World::~World() {
        cpSpaceFree(space);
    }

    void World::update() {
        cpSpaceStep(space, timeStep);
    }
    World& World::add_agent(Agent& agent) {
        agents.push_back(&agent); 
        return *this;
    }

    World& World::all(std::function<void(Agent&)> f) {
        for(auto agent_ptr : agents) {
            f(*agent_ptr);
        }
        return *this;            
    }

}