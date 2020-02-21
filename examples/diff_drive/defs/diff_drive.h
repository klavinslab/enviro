#ifndef __DIFF_DRIVE_AGENT__H
#define __DIFF_DRIVE_AGENT__H 

#include "agent.h"
#include "world.h"

using namespace enviro;

class DiffDrive : public Agent {

    public:
    DiffDrive(json spec, World& world);

    void init();
    void start() {}
    void update();
    void stop() {}

    high_resolution_clock::duration t;
    float dir;

};

DECLARE_INTERFACE(DiffDrive);

#endif