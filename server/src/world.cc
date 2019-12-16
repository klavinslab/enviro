#include <exception>
#include <dlfcn.h>
#include "world.h"

namespace enviro {

    World::World(std::string config_file_path) : Process("World") {

        space = cpSpaceNew();
        timeStep = 1.0/60.0;

        // read file
        std::ifstream ifs(config_file_path);
        if ( ifs.fail() ) {
            std::cerr << "Could not open configuration file " 
                      << config_file_path 
                      << "\n";
            throw std::runtime_error("file error");
        }

        json config;
        ifs >> config;

        // Todo: make sure there is a "name" key
        std::cout << "name = " << config["name"] << "\n";
        set_name(config["name"]);

        for ( auto agent_specification : config["agents"] ) {

            auto agent_ptr = CreateAgent(agent_specification, *this);
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