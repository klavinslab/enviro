#ifndef __ENVIRO_STATIC_OBJECT__H
#define __ENVIRO_STATIC_OBJECT__H 

#include "agent.h"
#include "world.h"

using namespace enviro;

class StaticObject : public Agent {

    public:
    StaticObject(json spec, World& world);
    void init() {}
    void start() {}
    void update() {}
    void stop() {}

    static json build_specification(json static_entry);    

};

#endif