#include <iostream>
#include "wanderer.h"

using namespace enviro;

Wanderer::Wanderer(json spec, World& world) : Agent(spec ,world){
    std::cout << "Constructed a DiffDrive Agent\n";
}

void Wanderer::init() {
    std::cout << "Starting DiffDrive Agent\n";
    t = last_update();
    dir = 1;
}

void Wanderer::update() {

    if ( last_update() - t > 4_s ) {
        t = last_update();
        dir = -dir;
    }
    if ( dir > 0 ) {
        servo(4*dir,0);
    } else {
        servo(0, 0.1*dir);
    }

}
