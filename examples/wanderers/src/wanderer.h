#ifndef __WANDERER_AGENT__H
#define __WANDERER_AGENT__H 

#include <string>
#include "enviro.h"

using namespace enviro;

class Wanderer : public Agent {

    public:
    Wanderer(json spec, World& world);

    void init();
    void start() {}
    void update();
    void stop() {}

    high_resolution_clock::duration t;
    double fwd, rot;
    std::string state;

};

DECLARE_INTERFACE(Wanderer);

#endif