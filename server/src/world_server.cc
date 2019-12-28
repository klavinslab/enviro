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
        port(port) {}

    void WorldServer::run() {

        uWS::SSLApp app = uWS::SSLApp()    // TODO: cast insteead of wrap where
          .get("/state", [&](auto *res, auto *req) { get_state(res,req); })
          .listen(port,  [&](auto *token)          { listen(token);      })
          .run();

        throw std::runtime_error("Server run returned, which it shouldn't do.");

    }

    void WorldServer::get_state(uWS::HttpResponse<true> *res, uWS::HttpRequest *req) {

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
        
        res->writeHeader("Access-Control-Allow-Origin", "*");
        res->end(result.dump().c_str());

    } 

    void WorldServer::listen(us_listen_socket_t * token) {
        if (token) {
            std::cout << "Listening on port " << port << std::endl;
        } else {
            std::cout << "failed to listen" << std::endl;
        }
    }

}