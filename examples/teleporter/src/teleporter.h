#ifndef __TELEPORTER_AGENT__H
#define __TELEPORTER_AGENT__H 

#include "enviro.h"

using namespace enviro;

class TeleporterController : public Process, public AgentInterface {

    public:
    TeleporterController();

    void init() {}
    void start() {}
    void update();
    void stop() {}

};

class Teleporter : public Agent {
    public:
    Teleporter(json spec, World& world) : Agent(spec, world) {
        add_process(bc);
    }
    private:
    TeleporterController bc;

};

DECLARE_INTERFACE(Teleporter)

#endif