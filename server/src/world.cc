#include <exception>
#include <dlfcn.h>
#include "enviro.h"

namespace enviro {

    World::World(json config) : Process("World") {

        space = cpSpaceNew();
        timeStep = 1.0/60.0; // TODO: move to config.json
        set_name(config["name"]);

        for ( auto agent_entry : config["agents"] ) {
            json spec = Agent::build_specification(agent_entry);
            auto agent_ptr = Agent::create_from_specification(spec, *this);
            add_agent(*agent_ptr);
        }

        for ( auto static_entry : config["statics"] ) {
            json spec = StaticObject::build_specification(static_entry);
            auto agent_ptr = new StaticObject(spec, *this);
            add_agent(*agent_ptr);
        }        

    }

    World::~World() {
        cpSpaceFree(space);
        for ( auto agent_ptr : agents ) {
            agent_ptr->_destroyer(agent_ptr);
        }
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