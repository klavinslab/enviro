#include <iostream>
#include "diff_drive.h"

using namespace enviro;

DiffDrive::DiffDrive(json spec, World& world) : Agent(spec ,world){
    std::cout << "Constructed a DiffDrive Agent\n";
}

void DiffDrive::initialize() {
    std::cout << "Starting DiffDrive Agent";
}

void DiffDrive::step() {
    std::cout << "Stepping DiffDrive Agent";
}


