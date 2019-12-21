#include <iostream>
#include "diff_drive.h"

using namespace enviro;

DiffDrive::DiffDrive(json spec, World& world) : Agent(spec ,world){
    std::cout << "Constructed a DiffDrive Agent\n";
}

void DiffDrive::init() {
    std::cout << "Starting DiffDrive Agent\n";
    t = last_update();
    dir = 1;
}

void DiffDrive::update() {

    if ( last_update() - t > 4_s ) {
        t = last_update();
        dir = -dir;
    }
    if ( dir > 0 ) {
        servo(2*dir,0);
    } else {
        servo(0, 0.05*dir);
    }


}


