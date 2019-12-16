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

            std::cout << agent_specification["type"] << "\n";

            std::ifstream ifs(agent_specification["type"]);
            if ( ifs.fail() ) {
                std::cerr << "Could not open agent definition file " 
                        << agent_specification["type"]
                        << "\n";
                throw std::runtime_error("file error");
            }        

            json spec;
            ifs >> spec;            

            auto file = spec["controller"].get<std::string>().c_str();
            std::cout << file << "\n";
            auto handle = dlopen(file , RTLD_LAZY);
            if (!handle) {
                std::cerr << "Cannot open library " << dlerror() << '\n';
                throw std::runtime_error("dll error");
            }
            Agent* (*create_agent)(std::string, World&);
            void (*destroy_agent)(Agent*);
            std::cout << "A\n";
            create_agent = (Agent* (*)(std::string, World&))dlsym(handle, "create_agent");
            std::cout << "B\n";
            destroy_agent = (void (*)(Agent*))dlsym(handle, "destroy_agent");
            std::cout << "C\n";              
            auto agent_ptr = (Agent*) create_agent("asd", *this);
            std::cout << "D\n";
            add_agent(*agent_ptr);
            std::cout << "E\n";
        }

    }

    World::~World() {
        cpSpaceFree(space);
        // delete all agents
        for ( auto agent_ptr : agents ) {
            delete agent_ptr;
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