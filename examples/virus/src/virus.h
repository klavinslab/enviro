#ifndef __VIRUS_AGENT__H
#define __VIRUS_AGENT__H 

#include "enviro.h"
#include <math.h>

using namespace enviro;

class VirusController : public Process, public AgentInterface {

    public:
    VirusController() : Process(), AgentInterface(), host_id(-1) {}

    void init() {
        notice_collisions_with("Cell", [&](Event &e) {
            host_id = e.value()["id"];
            Agent &host = find_agent(host_id);
            attach_to(host);
            host.set_style(INFECTED_HOST_STYLE);
            ignore_collisions_with("Cell");
            counter = 0;
        });            
    }

    void start() {}

    void infect_host() {
        Agent& host = find_agent(host_id);
        for ( double theta = 0; theta < 2*M_PI; theta += M_PI/8 ) {
            Agent& v = add_agent("Virus", host.x() + 90*cos(theta), host.y() + 90*sin(theta), 0, VIRUS_STYLE);
        }
        remove_agent(host_id);
        remove_agent(id());
    }

    void update() {
        omni_apply_force(
            (rand() % fmax) - fmax/2, 
            (rand() % fmax) - fmax/2
        );
        if ( host_id >= 0 && counter++ == 100 ) {
            if ( agent_exists(host_id) ) {
                infect_host();
            } 
        } else if ( host_id > 0 && !agent_exists(host_id) ) {
            remove_agent(id());
        }
    }

    void stop() {}

    const int fmax = 100.0;
    int host_id;
    int counter;

    const json INFECTED_HOST_STYLE = { 
                   {"fill", "lightgreen"}, 
                   {"stroke", "#aaa"}, 
                   {"strokeWidth", "10px"},
                   {"strokeOpacity", "0.25"}
               },
               VIRUS_STYLE = { 
                   {"fill", "orange"}, 
                   {"stroke", "black"}, 
                   {"strokeWidth", "10px"},
                   {"strokeOpacity", "0.25"}
               };            

};

class Virus : public Agent {
    public:
    Virus(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    VirusController c;
};

DECLARE_INTERFACE(Virus)

#endif
