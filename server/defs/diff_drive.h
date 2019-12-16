#ifndef __DIFF_DRIVE_AGENT__H
#define __DIFF_DRIVE_AGENT__H 

#include "enviro.h"

namespace enviro {

    class World;

    class DiffDrive : public Agent {

        public:
        DiffDrive(std::string name, World& world);

        void initialize();
        void step();

    };

}

#endif