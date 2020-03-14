#ifndef __ENVIRO_WORLD__H
#define __ENVIRO_WORLD__H 

#include <iostream>
#include <chrono>
#include <tuple>
#include "elma/elma.h"
#include "chipmunk.h"
#include "enviro.h"

using namespace std::chrono;
using namespace elma;

namespace enviro {

    class Agent;

    typedef struct {
        json specification;
        void * handle;
        Agent* (*create_agent)(json spec, World&);
        void (*destroy_agent)(Agent*);
    } AGENT_TYPE;     

    class World : public Process {
        public:

        World(json config, Manager& m);
        ~World();

        void init();
        void start() {}
        void update();
        void stop() {}

        inline cpSpace * get_space() { return space; }
        World& add_agent(Agent& agent);
        Agent& add_agent(const std::string name, double x, double y, double theta, const json style);
        World& all(std::function<void(Agent&)> f);
        inline json get_config() const { return config; }
        Agent& find_agent(int id);
        void add_constraint(Agent& a, Agent& b);
        bool attached(Agent& a, Agent& b);
        bool exists(int id);
        void remove(int id);
        void remove_constraints_involving(int id);
        void process_removals();
        void add_agent_type(std::string name, AGENT_TYPE * at);
        AGENT_TYPE * add_agent_type(json spec);

        inline void set_center(double x, double y) { center_x = x; center_y = y; }
        inline void set_zoom(double z) { zoom = z; }
        inline double get_center_x() { return center_x; }
        inline double get_center_y() { return center_y; }
        inline double get_zoom() { return zoom; }

        private:
        map<std::string, AGENT_TYPE *> agent_types;
        vector<Agent *> agents, new_agents, garbage;
        cpSpace * space;
        cpFloat timeStep;
        json config;
        cpCollisionHandler * collsion_handler;
        Manager * manager_ptr;
        double center_x, center_y, zoom;

        // A pin joint connecting agents with the given ids.
        typedef std::tuple<int, int, cpConstraint*> Constraint;
        vector<Constraint> new_constraints, constraints;       

    };

}

#endif