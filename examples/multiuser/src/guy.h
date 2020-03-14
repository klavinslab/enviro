#ifndef __PLAYER_AGENT__H
#define __PLAYER_AGENT__H 

#include "enviro.h"

using namespace enviro;

class GuyController : public Process, public AgentInterface {

    public:
    GuyController() : Process(), AgentInterface(), f(0), tau(0), firing(false) {}

    void init() {
        watch("keydown", [&](Event &e) {
            if ( e.value()["client_id"] == get_client_id() ) {
                auto k = e.value()["key"].get<std::string>();
                if ( k == " " && !firing ) {
                    Agent& bullet = add_agent("Bullet", 
                        x() + 17*cos(angle()), 
                        y() + 17*sin(angle()), 
                        angle(), 
                        BULLET_STYLE);    
                        bullet.apply_force(50,0);
                    firing = true;
                } else if ( k == "w" ) {
                    f = magnitude;              
                } else if ( k == "s" ) {
                    f = -magnitude;  
                } else if ( k == "a" ) {
                    tau = -magnitude;
                } else if ( k == "d" ) {
                    tau = magnitude;
                } 
            }
        });        
        watch("keyup", [&](Event &e) {
            if ( e.value()["client_id"] == get_client_id() ) {            
                auto k = e.value()["key"].get<std::string>();
                if ( k == " " ) {
                    firing = false;
                } else if ( k == "w" || k == "s" ) {
                    f = 0;               
                } else if ( k == "a" ) {
                    tau = 0;
                } else if ( k == "d" ) {
                    tau = 0;
                } 
            }
        });
    }
    void start() {}
    void update() {
        apply_force(f,tau);
        label(std::to_string((int) x()) + ", " + std::to_string((int)y()),20,20);
    }
    void stop() {}

    double f, tau;
    double const magnitude = 200;
    bool firing;
    const json BULLET_STYLE = { 
                   {"fill", "green"}, 
                   {"stroke", "#888"}, 
                   {"strokeWidth", "5px"},
                   {"strokeOpacity", "0.25"}
               };

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