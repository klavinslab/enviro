#ifndef __THING_AGENT__H
#define __THING_AGENT__H 

#include "enviro.h"

using namespace enviro;

class ThingController : public Process, public AgentInterface {

    public:
    ThingController() : Process(), AgentInterface() {}

    void init() {
        if ( id() == 0 ) {
          watch("button_click", [&](Event& e) {
              if ( e.value()["value"] == "zoom_in" ) {
                  z *= 2;
                  zoom(z);
              } else if ( e.value()["value"] == "zoom_out" ) {
                  z /= 2;
                  zoom(z);
              } else if ( e.value()["value"] == "center_one" ) {
                  center(-0,0);
              } else if ( e.value()["value"] == "center_two" ) {
                  center(40,-10);
              }
          });
          watch("screen_click", [&](Event& e) {
              std::cout << "clicked screen at " << e.value()["x"] << ", " << e.value()["y"] << "\n";
          });
        }
        watch("agent_click", [&](Event& e) {
            int agent_id = e.value()["id"];
            if ( id() == agent_id ) {
                std::cout << "clicked agent " << agent_id << ", " << e.value()["x"] << ", " << e.value()["y"] << "\n";
            }
        });
        // label(std::to_string((int) x()) + ", " + std::to_string((int) y()), 15, 15);
    }

    void start() {}
    void update() {}
    void stop() {}

    double z = 1.0;

};

class Thing : public Agent {
    public:
    Thing(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    ThingController c;
};

DECLARE_INTERFACE(Thing)

#endif