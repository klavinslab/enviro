#ifndef __CHASER_AGENT__H
#define __CHASER_AGENT__H 

#include "enviro.h"

using namespace enviro;

class ChaserController : public Process, public AgentInterface {

    public:
    ChaserController() : Process(), AgentInterface(), z(1), tracking(false) {}

    void init() {
        goal_x = 0;
        goal_y = 0;
        watch("goal_change", [this](Event e) {
            goal_x = e.value()["x"];
            goal_y = e.value()["y"];
            std::cout << "New goal: " << goal_x << ", " << goal_y << "\n";
        });
        watch("button_click", [&](Event& e) {
            if ( e.value()["value"] == "zoom_in" ) {
                z *= 2;
                zoom(z);
            } else if ( e.value()["value"] == "zoom_out" ) {
                z /= 2;
                zoom(z);
            } else if ( e.value()["value"] == "toggle_track" ) {
                tracking = !tracking;
                if ( !tracking ) {
                    center(0,0);
                }
            }
        }); 
    }
    void start() {}
    void update() {
        move_toward(goal_x, goal_y);
        if ( tracking ) {
            center(x(), y());
        }
    }
    void stop() {}

    double goal_x, goal_y;
    double z;
    bool tracking;

};

class Chaser : public Agent {
    public:
    Chaser(json spec, World& world) : Agent(spec, world) {
        add_process(bc);
    }
    private:
    ChaserController bc;
};

DECLARE_INTERFACE(Chaser)

#endif