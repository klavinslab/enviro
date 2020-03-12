#include <exception>
#include <dlfcn.h>
#include "enviro.h"

namespace enviro {

    World::World(json config, Manager& m) : Process("World"), config(config), manager_ptr(&m) {

        space = cpSpaceNew();
        timeStep = 1.0/60.0; // TODO: move to config.json
        set_name(config["name"]);

        for ( auto agent_entry : config["agents"] ) {
            json spec = Agent::build_specification(agent_entry);
            AGENT_TYPE * at = add_agent_type(spec);
            auto agent_ptr = at->create_agent(spec, *this); 
            agent_ptr->set_destroyer(at->destroy_agent);
            add_agent(*agent_ptr);
        }

        for ( auto agent_entry : config["references"] ) {
            json spec = Agent::build_specification(agent_entry);
            AGENT_TYPE * at = add_agent_type(spec);
        }        

        for ( auto agent_entry : config["invisibles"] ) {
            json spec = Agent::build_specification(agent_entry);
            AGENT_TYPE * at = add_agent_type(spec);
            auto agent_ptr = at->create_agent(spec, *this); 
            agent_ptr->set_destroyer(at->destroy_agent);
            add_agent(*agent_ptr);
        }            

        for ( auto static_entry : config["statics"] ) {
            json spec = StaticObject::build_specification(static_entry);
            auto agent_ptr = new StaticObject(spec, *this);
            add_agent(*agent_ptr);
        }        

    }

    Agent& World::add_agent(const std::string name, double x, double y, double theta, const json style) {      

        if ( agent_types.find(name) == agent_types.end() ) {
            throw std::runtime_error("Could not add new agent. Unknown type.");
        } 

        auto at = agent_types[name];
        json new_spec = at->specification;
        new_spec["position"]["x"] = x;
        new_spec["position"]["y"] = y;
        new_spec["position"]["theta"] = theta;
        new_spec["style"] = style;

        auto agent_ptr = at->create_agent(new_spec, *this); 
        agent_ptr->set_destroyer(at->destroy_agent);
        new_agents.push_back(agent_ptr);
        agent_ptr->set_manager(manager_ptr);
        agent_ptr->init(); 
        agent_ptr->start();
        return *agent_ptr;
    }

    AGENT_TYPE * World::add_agent_type(json spec) {

        std::string name = spec["definition"]["name"];
        AGENT_TYPE * at;

        if ( agent_types.find(name) != agent_types.end() ) {
            at = agent_types[name];
        } else {
            auto file = spec["definition"]["controller"].get<std::string>();
            at = new AGENT_TYPE;
            at->specification = spec;
            at->handle = dlopen(file.c_str() , RTLD_LAZY);
            if (!at->handle) {
                std::cerr << "Error: " << file << "\n";
                throw std::runtime_error(dlerror());
            }
            at->create_agent = AGENT_CREATE_TYPE dlsym(at->handle, "create_agent");
            at->destroy_agent = AGENT_DESTROY_TYPE dlsym(at->handle, "destroy_agent");
            agent_types[name] = at;
        } 

        return at;

    }    

    World::~World() {
        cpSpaceFree(space);
        for ( auto agent_ptr : agents ) {
            agent_ptr->_destroyer(agent_ptr);
        }
    }

    static cpBool handle_collision(cpArbiter *arb, cpSpace *space, void *data) {
        cpBody *a, *b;
        cpArbiterGetBodies(arb, &a, &b);
        Agent * A = (Agent*) cpBodyGetUserData(a),
              * B = (Agent*) cpBodyGetUserData(b);
        
        A->handle_collision(*B);
        B->handle_collision(*A);

        return cpTrue;
    }

    void World::init() {
        collsion_handler = cpSpaceAddCollisionHandler(space, AGENT_COLLISION_TYPE, AGENT_COLLISION_TYPE);
        collsion_handler->beginFunc = handle_collision;
    }

    void World::update() {
        // std::cout << "A\n";
        for ( auto c : new_constraints ) {
             cpSpaceAddConstraint(space, std::get<2>(c));
             constraints.push_back(c);
        }
        // std::cout << "B\n";
        new_constraints.erase(new_constraints.begin(), new_constraints.end());
        // std::cout << "C\n";
        for ( auto agent_ptr : garbage ) {
            delete agent_ptr;
        }
        garbage.erase(garbage.begin(), garbage.end());
        process_removals(); // removes agents from manager, but manager is still going through agents
        // std::cout << "D\n";
        for ( auto agent_ptr : new_agents ) {
            add_agent(*agent_ptr);
            manager_ptr->add(*agent_ptr, 100_ms);
        }
        // std::cout << "E\n";
        new_agents.erase(new_agents.begin(), new_agents.end());
        // std::cout << "F\n";
        cpSpaceStep(space, timeStep);
        // std::cout << "G\n";
    }

    World& World::add_agent(Agent& agent) {
        agents.push_back(&agent); 
        return *this;
    }

    void World::add_agent_type(std::string name, AGENT_TYPE * at) {
        if ( agent_types.find(name) != agent_types.end() ) {
            agent_types[name] = at;
        }
    }

    World& World::all(std::function<void(Agent&)> f) {
        for(auto agent_ptr : agents) {
            f(*agent_ptr);
        }
        return *this;            
    }

    Agent& World::find_agent(int id) {
        for(auto agent_ptr : agents) {
            if ( agent_ptr->get_id() == id ) {
                return *agent_ptr;
            }
        }
        std::string msg = "Could not find agent with id ";
        msg += id;
        throw std::runtime_error(msg);
    }

    void World::add_constraint(Agent& a, Agent& b) {
        if ( ! attached(a,b) ) {
            cpConstraint * c = cpPinJointNew(a._body, b._body, cpvzero, cpvzero);
            new_constraints.push_back(std::make_tuple(a.get_id(),b.get_id(),c));
        }
    }

    bool World::attached(Agent& a, Agent& b) {
        for ( auto c : constraints ) {
            if ( std::get<0>(c) == a.get_id() && std::get<1>(c) == b.get_id() ) {
                return true;
            }
        }
        return false;
    }

    bool World::exists(int id) {
        for ( Agent * a : agents ) {
            if ( a->get_id() == id ) {
                return true;
            }
        }
        return false;
    }

    void World::remove(int id) {
        std::cout << "marking " << id << " for removal" << "\n";
        find_agent(id).mark_for_removal();
    }

    void World::remove_constraints_involving(int id) {
       auto j = std::remove_if(constraints.begin(), constraints.end(), [&](Constraint c) {
            bool found = (std::get<0>(c) == id || std::get<1>(c) == id);
            if ( found ) {
                auto constraint_ptr = std::get<2>(c);
                cpSpaceRemoveConstraint(space, constraint_ptr);
                cpConstraintDestroy(constraint_ptr);
                cpConstraintFree(constraint_ptr);
            }
            return found;
        }); 
        constraints.erase(j,constraints.end()); 
    }

    void World::process_removals() {

        // remove from agents list
        auto i = std::remove_if(agents.begin(), agents.end(), [&](Agent *a) {
            if ( !a->is_alive() ) {
                // remove all constraints involving the agent,
                // remove its body and shape from the physics engine,
                // remove it from the manager, and then destroy it
                remove_constraints_involving(a->get_id());
                cpSpaceRemoveShape(space, a->_shape);
                cpSpaceRemoveBody(space, a->_body);
                manager_ptr->remove(*a);
                garbage.push_back(a);
            }
            return !a->is_alive();
        });
        
        // delete the agent pointers 
        agents.erase(i, agents.end());           

    }

}