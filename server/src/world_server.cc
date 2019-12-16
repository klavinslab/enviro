#include "world_server.h"

namespace enviro {

    long int unix_timestamp() {
        time_t t = std::time(0);
        long int now = static_cast<long int> (t);
        return now;
    }    

    WorldServer::WorldServer(World& world, std::mutex& mutex, const char* ip, int port) 
        : world(world), 
        manager_mutex(mutex),
        ip(ip), 
        port(port)
    {        
        server.Get("/state", [&](const Request& req, Response& res) {
            get_state(req, res);
        });
    }

    void WorldServer::run() {
        server.listen(ip, port);
    }  

    void WorldServer::stop() {
        server.stop();
    }       

    void WorldServer::get_state(const Request& req, Response& res) {

        json agent_list;
        
        manager_mutex.lock(); ///////////////////////////////////////////////        
        world.all([&](Agent& agent) {                                      //
            agent_list.push_back(agent.serialize());                       //
        });                                                                //
        manager_mutex.unlock(); /////////////////////////////////////////////

        json result = {
            { "result", "ok" },
            { "timestamp", unix_timestamp() },
            { "agents", agent_list }
        };
        
        res.set_header( "Access-Control-Allow-Origin", "*" );
        res.set_content(result.dump(), "json");

    }

}