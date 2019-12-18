#include <iostream>
#include <chrono>

#include "elma.h"
#include "enviro.h"

//! \filee

using namespace std::chrono;
using namespace elma;
using namespace enviro;

int main() {

    Manager m;
    World world("config.json");
    WorldServer world_server(
        world, 
        m.get_update_mutex(), 
        "0.0.0.0", // Todo: Move ip and port to config.json 
        8765       //       or command line
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

    m.run(6000_s);

    world_server.stop();
    server_thread.join();
     
}
