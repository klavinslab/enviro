#ifndef __PLAYER_AGENT__H
#define __PLAYER_AGENT__H 

#include "enviro.h"

using namespace enviro;

class PlayerController : public Process, public AgentInterface {

    public:
    PlayerController() : Process(), AgentInterface(), f(0), tau(0) {}

    void init() {
        watch("keydown", [&](Event &e) {
            auto k = e.value()["key"].get<std::string>();
            if ( k == " " ) {
                  std::cout << "fire!\n";
            } else if ( k == "w" ) {
                  f = magnitude;              
            } else if ( k == "s" ) {
                  f = -magnitude;  
            } else if ( k == "a" ) {
                  tau = -magnitude;
            } else if ( k == "d" ) {
                  tau = magnitude;
            } 
        });        
        watch("keyup", [&](Event &e) {
            auto k = e.value()["key"].get<std::string>();
            if ( k == "w" || k == "s" ) {
                  f = 0;               
            } else if ( k == "a" ) {
                  tau = 0;
            } else if ( k == "d" ) {
                  tau = 0;
            } 
        });
    }
    void start() {}
    void update() {
        apply_force(f,tau);
    }
    void stop() {}

    double f, tau;
    double const magnitude = 200;

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