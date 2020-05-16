#include "NetworkClient.hpp"

extern "C" {
    #include "client.h"
}

#pragma once

class ServerInfoRequest {
public:
    static int request(std::string address, PSERVER_DATA server);
};
