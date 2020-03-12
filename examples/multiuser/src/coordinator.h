#ifndef __COORDINATOR_AGENT__H
#define __COORDINATOR_AGENT__H 

#include "enviro.h"

using namespace enviro;

class CoordinatorController : public Process, public AgentInterface {

    public:
    CoordinatorController() : Process(), AgentInterface() {}

    void init() {
        watch("connection", [&](Event e) {
            std::cout << "Connection from " << e.value()["id"] << "\n";
            Agent& a = add_agent("Guy", 0, y, 0, {{"fill","gray"},{"stroke","black"}});
            a.set_client_id(e.value()["id"]);
            y += 50;
        });
    }
    void start() {}
    void update() {}
    void stop() {}

    double y = -150;

};

class Coordinator : public Agent {
    public:
    Coordinator(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    CoordinatorController c;
};

DECLARE_INTERFACE(Coordinator)

#endif