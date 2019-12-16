#ifndef __DIFF_DRIVE_AGENT__H
#define __DIFF_DRIVE_AGENT__H 

#include "enviro.h"

using namespace enviro;

class Block : public Agent {

    public:
    Block(std::string name, World& world);
    void initialize();
    void step();

};

DECLARE_INTERFACE(Block)

#endif