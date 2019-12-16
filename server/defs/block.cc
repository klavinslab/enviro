#include <iostream>
#include "block.h"

using namespace enviro;

Block::Block(std::string name, World& world) : Agent(name,world) {
    std::cout << "Constructed a Block Agent\n";
}

void Block::initialize() {
    std::cout << "Starting a Block Agent";
}

void Block::step() {
    std::cout << "Stepping a Block Agent";
}
