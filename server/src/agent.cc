#include <dlfcn.h>
#include "agent.h"

namespace enviro {

    Agent::Agent(std::string name, World& world) : Process(name) {
        cpSpace * space = world.get_space();
        cpFloat moment = cpMomentForCircle(1, 0, 5, cpvzero);            
        _body = cpSpaceAddBody(space, cpBodyNew(1, moment));
        cpBodySetPosition(_body, cpv(0, 15));
        _shape = cpSpaceAddShape(space, cpCircleShapeNew(_body, 5, cpvzero));
        cpShapeSetFriction(_shape, 0.7);         
    }

    Agent::~Agent() {
        cpShapeFree(_shape);
        cpBodyFree(_body);
    }    

    json Agent::serialize() {
        cpVect pos = cpBodyGetPosition(_body);
        cpVect vel = cpBodyGetVelocity(_body);
        return {
            {"id", 0}, // todo: give agents ids
            {"x", pos.x},
            {"y",pos.y},
            {"dx", vel.x},
            {"dy", vel.y}
        };            
    }

    // Factory methods /////////////////////////////////////////

    Agent * CreateAgent(json specification, World& world) {

        std::ifstream ifs(specification["type"]);
        if ( ifs.fail() ) {
            std:string msg = "Could not open agent definition file ";
            msg += specification["type"];
            throw std::runtime_error(msg);
        }

        json spec;
        ifs >> spec;

        auto file = spec["controller"].get<std::string>().c_str();
        auto handle = dlopen(file , RTLD_LAZY);

        if (!handle) {
            throw std::runtime_error(dlerror());
        }
        
        auto create_agent = AGENT_CREATE_TYPE  dlsym(handle, "create_agent");
        auto agent_ptr = (Agent*) create_agent("temporary name", world); // TODO: Use the json to build the agent's chipmunk parts 
        agent_ptr->set_destroyer(AGENT_DESTROY_TYPE dlsym(handle, "destroy_agent"));

        return agent_ptr;

    }

}