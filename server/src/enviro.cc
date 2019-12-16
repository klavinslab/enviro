#include <iostream>
#include <chrono>
#include <stdlib.h>

#include "elma.h"
#include "chipmunk.h"
#include "agent.h"
#include "world.h"
#include "world_server.h"

//! \filee

using namespace std::chrono;
using namespace elma;
using namespace enviro;

// class RandomWalker : public Agent {

//     public:

//     RandomWalker(std::string name, World& world) : Agent(name, world) {}

//     void init() {}
//     void start() {}
//     void update() {
//         cpVect f = {
//             0.001*(rand()%100), 
//             0.001*(rand()%100)
//         };
//         cpBodyApplyImpulseAtLocalPoint(body, f, cpvzero);
//         cpVect pos = cpBodyGetPosition(body);
//         // std::cout << pos.x << ", " << pos.y << "\n";
//     }
//     void stop() {}

// };

int main() {

    Manager m; 
    World world("config.json");
    WorldServer world_server(
        world, 
        m.get_update_mutex(), 
        "0.0.0.0", 
        8765
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

    server_thread.detach();

    m.run();
     
}
