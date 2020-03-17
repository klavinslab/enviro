#ifndef __THING_AGENT__H
#define __THING_AGENT__H 

#include "enviro.h"

using namespace enviro;

class ThingController : public Process, public AgentInterface {

    public:
    ThingController() : Process(), AgentInterface(), guy_x(0), guy_y(0) {}

    void init() {
        watch("guy_position", [&](Event& e) {
            guy_x  = e.value()["x"];
            guy_y  = e.value()["y"];
        });
    }
    void start() {}
    void update() {
        omni_move_toward(guy_x, guy_y, 0.1);
    }
    void stop() {}

    double guy_x, guy_y;

};

class Thing : public Agent {
    public:
    Thing(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    ThingController c;
};

DECLARE_INTERFACE(Thing)

#endif