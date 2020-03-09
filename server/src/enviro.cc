#include <iostream>
#include <chrono>

#include "elma/elma.h"
#include "enviro.h"
#include "world_server.h"

//! \filee

using namespace std::chrono;
using namespace elma;
using namespace enviro;

class DummyState : public State {
    void entry(const Event& e) { emit(Event("dummy")); }
    void during() {}
    void exit(const Event& e) {}
};

int main() {

    json config = json_helper::read("config.json");
    json_helper::check(config, ENVIRO_CONFIG_SCHEMA);

    Manager m;
    World world(config, m);
    StateMachine sm; // This is here just so the enviro executable includes
                     // state machines from libelma.a. Weird.
    DummyState state;

    WorldServer world_server(
        world,
        m.get_update_mutex(),
        config
    );

    m.use_real_time()
     .set_niceness(100_us)
     .schedule(world, 1_ms);

    world.all(
        [&](Agent& a) { 
            m.schedule(a, 100_ms);
        }
    );

    m.init();

    std::thread server_thread([&]() { 
        world_server.run(); 
    });

    m.run();
    server_thread.join();
     
}
