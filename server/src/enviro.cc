#include <iostream>
#include <chrono>

#include "elma/elma.h"
#include "enviro.h"
#include "world_server.h"

//! \filee

using namespace std::chrono;
using namespace elma;
using namespace enviro;

int main() {

    json config = json_helper::read("config.json");
    json_helper::check(config, ENVIRO_CONFIG_SCHEMA);

    Manager m;
    World world(config);

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
