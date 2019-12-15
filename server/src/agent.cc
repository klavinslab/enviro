#include "agent.h"

namespace enviro {

    Agent::Agent(std::string name, World& world) : Process(name) {
        cpSpace * space = world.get_space();
        cpFloat moment = cpMomentForCircle(1, 0, 5, cpvzero);            
        body = cpSpaceAddBody(space, cpBodyNew(1, moment));
        cpBodySetPosition(body, cpv(0, 15));
        shape = cpSpaceAddShape(space, cpCircleShapeNew(body, 5, cpvzero));
        cpShapeSetFriction(shape, 0.7);
        world.add_agent(*this);            
    }

    Agent::~Agent() {
        cpShapeFree(shape);
        cpBodyFree(body);
    }    

    json Agent::serialize() {
        cpVect pos = cpBodyGetPosition(body);
        cpVect vel = cpBodyGetVelocity(body);
        return {
            {"id", 0}, // todo: give agents ids
            {"x", pos.x},
            {"y",pos.y},
            {"dx", vel.x},
            {"dy", vel.y}
        };            
    }

}