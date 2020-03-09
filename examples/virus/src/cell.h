#ifndef __CELL_AGENT__H
#define __CELL_AGENT__H 

#include "enviro.h"

using namespace enviro;

class CellController : public Process, public AgentInterface {

    public:
    CellController() : Process(), AgentInterface() {}

    void init() {}

    void start() {}

    void update() {
        omni_apply_force(
            (rand() % fmax) - fmax/2, 
            (rand() % fmax) - fmax/2
        );
    }

    void stop() {}

    const int fmax = 100.0;

};

class Cell : public Agent {
    public:
    Cell(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    CellController c;
};

DECLARE_INTERFACE(Cell)

#endif