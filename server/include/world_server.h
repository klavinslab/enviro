#ifndef __ENVIRO_SERVER__H
#define __ENVIRO_SERVER__H

#include <iostream>
#include <ctime>
#include <map>
#include <string>
#include <mutex>

#include "json/json.h"
#include "agent.h"
#include "world.h"
#include "uWebSockets/App.h"

using namespace httplib;
using nlohmann::json; 

namespace enviro {

    long int unix_timestamp(); 
    class World;

    class WorldServer {

        public:
        WorldServer(World& world, std::mutex& mutex, const char* ip, int port);

        void get_state(uWS::HttpResponse<true> *res, uWS::HttpRequest *req);
        void listen(us_listen_socket_t * token);
        void run();
        void stop() {}

        private:
        World& world;
        std::mutex& manager_mutex;
        const char* ip;
        int port;

    };

}

#endif