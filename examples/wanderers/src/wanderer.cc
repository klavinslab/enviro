#include <iostream>
#include <math.h>
#include "wanderer.h"

using namespace enviro;

Wanderer::Wanderer(json spec, World& world) : Agent(spec ,world){
    std::cout << "Constructed a DiffDrive Agent\n";
}

void Wanderer::init() {
    std::cout << "Starting DiffDrive Agent\n";
    state = "forward";
    fwd = 1;
    rot = 0;
}

void Wanderer::update() {

    if ( state == "forward" && rand() % 100 <= 5 ) {
        state = "rotate";
        fwd = 0;
        rot = rand() % 2 == 0 ? -0.25 : 0.25;
    } else if ( state == "rotate" && rand() % 100 <= 5 ) {
        state = "forward";
        fwd = 2;
        rot = 0;
    }

    servo(fwd, rot);

}
