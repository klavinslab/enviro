#include "enviro.h"
#include "world_server.h"

namespace enviro {

    long int unix_timestamp() {
        time_t t = std::time(0);
        long int now = static_cast<long int> (t);
        return now;
    }    

    WorldServer::WorldServer(World& world, std::mutex& mutex, json config) 
        : world(world), 
        manager_mutex(mutex),
        ip(config["ip"].get<std::string>().c_str()),
        port(config["port"]) {}

    void WorldServer::run() {

        uWS::SSLApp app = uWS::SSLApp()    // TODO: cast insteead of wrap where
          .get("/config/:id", [&](auto *res, auto *req) { get_config(res,req); })
          .get("/state/:id",  [&](auto *res, auto *req) { get_state(res,req); })
          .post("/event",     [&](auto *res, auto *req) { process_client_event(res,req); })
          .listen(port,       [&](auto *token)          { listen(token);      })
          .run();

        throw std::runtime_error("Server run returned, which it shouldn't do.");

    }

    void WorldServer::get_config(uWS::HttpResponse<true> *res, uWS::HttpRequest *req) {
        
        json result = {
            { "result", "ok" },
            { "timestamp", unix_timestamp() },
            { "config", world.get_config() }
        };

        json event_data = { {"id", req->getParameter(0) }};
        manager_mutex.lock(); ///////////////////////////////////////////////        
        world.emit(Event("connection", event_data));                       //
        manager_mutex.unlock(); /////////////////////////////////////////////  

        res->writeHeader("Access-Control-Allow-Origin", "*");
        res->end(result.dump().c_str());

    }

    void WorldServer::get_state(uWS::HttpResponse<true> *res, uWS::HttpRequest *req) {

        json agent_list;
        
        manager_mutex.lock(); ///////////////////////////////////////////////        
        world.all([&](Agent& agent) {                                      //
            if ( agent.visible() ) {                                       //
                agent_list.push_back(agent.serialize());                   //
            }                                                              //
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

    void WorldServer::process_client_event(uWS::HttpResponse<true> *res, uWS::HttpRequest *req) {
        std::string buffer;
        res->onData([this,res,buffer=std::move(buffer)](std::string_view data, bool last) mutable {
            buffer.append(data.data(), data.length());
            if ( last ) {
                json data = json::parse(buffer);
                manager_mutex.lock(); ///////////////////////////////////////////////        
                world.emit(Event(data["type"], data));                             //
                manager_mutex.unlock(); /////////////////////////////////////////////            
            }
        });
        json result = {
            { "result", "ok" },
            { "timestamp", unix_timestamp() }
        };
        res->writeHeader("Access-Control-Allow-Origin", "*");
        res->end(result.dump().c_str());            
    }

    void WorldServer::listen(us_listen_socket_t * token) {
        if (token) {
            std::cout << "Listening on port " << port << std::endl;
        } else {
            throw std::runtime_error("WorldServer could not listen for connections.");
        }
    }

}