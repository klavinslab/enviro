#ifndef __DIFF_DRIVE_AGENT__H
#define __DIFF_DRIVE_AGENT__H 

#include "enviro.h"

using namespace enviro;

class DiffDrive : public Agent {

    public:
    DiffDrive(std::string name, World& world);

    void initialize();
    void step();

};

DECLARE_INTERFACE(DiffDrive);

#endif