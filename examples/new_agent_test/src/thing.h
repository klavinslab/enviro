#ifndef __THING_AGENT__H
#define __THING_AGENT__H 

#include "enviro.h"

using namespace enviro;

class Thing : public Agent {
    public:
    Thing(json spec, World& world) : Agent(spec, world) {}
    void start() {
        if ( get_id() == 0 ) {
            Agent& new_thing = add_agent("Thing", 1,2,3, {{"fill", "lightgreen"}});
        }
        Agent::start();
    }
};

DECLARE_INTERFACE(Thing)

#endif