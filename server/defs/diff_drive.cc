#include <iostream>
#include "diff_drive.h"

extern "C" enviro::DiffDrive* create_agent(std::string name, enviro::World& world) {
    std::cout << "X\n";
    return new enviro::DiffDrive(name, world);
}

extern "C" void destroy_agent( enviro::DiffDrive* object ) {
  delete object;
}

namespace enviro {

    DiffDrive::DiffDrive(std::string name, World& world) : Agent(name,world) {}

    void DiffDrive::initialize() {
        std::cout << "Starting DiffDrive Agent";
    }

    void DiffDrive::step() {
        std::cout << "Stepping DiffDrive Agent";
    }

}

