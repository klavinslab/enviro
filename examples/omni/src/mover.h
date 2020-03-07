#ifndef __MOVER_AGENT__H
#define __MOVER_AGENT__H 

#include "enviro.h"
#include <math.h>

using namespace enviro;

class MoverController : public Process, public AgentInterface {

    public:
    MoverController() : Process(), AgentInterface() {}

    void init() {}
    void start() {
        i = 0;
        t = 0;
    }
    void update() {
        t += 1;
        if ( t > 40 ) {
            t = 0;
            i = (i+1)%4;
        }
        omni_track_velocity(5*vx[i], 5*vy[i]);
    }
    void stop() {}

    int t;
    const vector<double> vx = { 1, 0, -1, 0 };
    const vector<double> vy = { 0, 1, 0, -1 };
    int i;

};

class Mover : public Agent {
    public:
    Mover(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    MoverController c;
};

DECLARE_INTERFACE(Mover)

#endif