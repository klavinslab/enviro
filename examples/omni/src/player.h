#ifndef __PLAYER_AGENT__H
#define __PLAYER_AGENT__H 

#include "enviro.h"

using namespace enviro;

class PlayerController : public Process, public AgentInterface {

    public:
    PlayerController() : Process(), AgentInterface() {}

    void init() {
        watch("keyup", [&](Event &e) {
            auto k = e.value()["key"].get<std::string>();
            if ( k == "w" ) {
                  fx = 0;
                  fy = -force;
            } else if ( k == "s" ) {
                  fx = 0;
                  fy = force;
            } else if ( k == "a" ) {
                  fx = -force;
                  fy = 0;
            } else if ( k == "d" ) {
                  fx = force;
                  fy = 0;
            } else {
                fx = 0;
                fy = 0;
            }
            omni_apply_force(fx,fy);
            
        });
    }

    void start() {
        fx = 0;
        fy = 0;
    }

    void update() {
        omni_damp_movement();
    }

    void stop() {}

    double fx, fy;

    const double force = 100;

};

class Player : public Agent {
    public:
    Player(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    PlayerController c;
};

DECLARE_INTERFACE(Player)

#endif