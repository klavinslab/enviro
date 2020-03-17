#ifndef __VIRUS_FRAGMENT_AGENT__H
#define __VIRUS_FRAGMENT_AGENT__H 

#include "enviro.h"
#include <math.h>

using namespace enviro;

class VirusFragmentController : public Process, public AgentInterface {

    public:
    VirusFragmentController() : Process(), AgentInterface() {}

    void init() {
        counter = rand() % 8;
    }
    void start() {}
    void update() {
        // omni_apply_force(
        //     (rand() % fmax) - fmax/2, 
        //     (rand() % fmax) - fmax/2
        // );     
        if ( counter-- <= 0 ) {
            remove_agent(id());
        }
    }
    void stop() {}
    int counter;
    const int fmax = 10;

};

class VirusFragment : public Agent {
    public:
    VirusFragment(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    VirusFragmentController c;
};

DECLARE_INTERFACE(VirusFragment)

#endif