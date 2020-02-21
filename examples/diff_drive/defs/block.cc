#include <iostream>
#include "block.h"

using namespace enviro;

Block::Block(json spec, World& world) : Agent(spec,world) {
    std::cout << "Constructed a Block Agent\n";
}

void Block::update() {
    damp_movement();
}
