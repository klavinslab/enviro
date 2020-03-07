#ifndef __TRACKER_AGENT__H
#define __TRACKER_AGENT__H 

#include "enviro.h"

using namespace enviro;

class TrackerController : public Process, public AgentInterface {

    public:
    TrackerController() : Process(), AgentInterface() {}

    void init() {
        watch("screen_click", [&](Event &e) {
            teleport(e.value()["x"], e.value()["y"],0);
        });
    }
    void start() {}
    void update() {
        omni_move_toward(0,0,0.75);
    }
    void stop() {}

};

class Tracker : public Agent {
    public:
    Tracker(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    TrackerController c;
};

DECLARE_INTERFACE(Tracker)

#endif