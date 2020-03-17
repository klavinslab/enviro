#ifndef __PLAYER_AGENT__H
#define __PLAYER_AGENT__H 

#include "enviro.h"

using namespace enviro;

class GuyController : public Process, public AgentInterface {

    public:
    GuyController() : Process(), AgentInterface(), v(0), omega(0) {}

    void init() {
        watch("keydown", [&](Event &e) {
            auto k = e.value()["key"].get<std::string>();
            if ( k == "w" ) {
                  v = v_m;              
            } else if ( k == "s" ) {
                  v = -v_m;  
            } else if ( k == "a" ) {
                  omega = -omega_m;
            } else if ( k == "d" ) {
                  omega = omega_m;
            } 
        });        
        watch("keyup", [&](Event &e) {
            auto k = e.value()["key"].get<std::string>();
            if ( k == "w" || k == "s" ) {
                  v = 0;               
            } else if ( k == "a" ) {
                  omega = 0;
            } else if ( k == "d" ) {
                  omega = 0;
            } 
        });
    }
    void start() {}
    void update() {
        track_velocity(v,omega,10,400);
        label(std::to_string((int) x()) + ", " + std::to_string((int)y()),20,20);
        emit(Event("guy_position", { { "x", x()}, {"y", y() }}));
    }
    void stop() {}

    double v, omega;
    double const v_m = 30, omega_m = 1;
    double const magnitude = 200;

};

class Guy : public Agent {
    public:
    Guy(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    GuyController c;
    
};

DECLARE_INTERFACE(Guy)

#endif