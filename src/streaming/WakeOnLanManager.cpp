#include "WakeOnLanManager.hpp"
#include "Data.hpp"
#include "Logger.hpp"
#include "Settings.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

static const int numPorts = 5;
static const int ports[numPorts] = {7, 9, 47998, 47999, 48000};

static void populate_address(struct sockaddr_storage* addr, unsigned short port) {
    if (addr->ss_family == AF_INET) {
        struct sockaddr_in *sin = (struct sockaddr_in*)addr;
        sin->sin_port = htons(port);
    }
    else if (addr->ss_family == AF_INET6) {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)addr;
        sin6->sin6_port = htons(port);
    }
}

static Data mac_string_to_bytes(std::string mac) {
    std::string str = mac;
    std::string pattern = ":";
    
    std::string::size_type i = str.find(pattern);
    while (i != std::string::npos) {
        str.erase(i, pattern.length());
        i = str.find(pattern, i);
    }
    return Data((unsigned char*)str.c_str(), str.length()).hex_to_bytes();
}

static Data create_payload(const Host &host) {
    Data payload;
    
    // 6 bytes of FF
    uint8_t header = 0xFF;
    for (int i = 0; i < 6; i++) {
        payload = payload.append(Data(&header, 1));
    }
    
    // 16 repitiions of MAC address
    Data mac_address = mac_string_to_bytes(host.mac);
    for (int i = 0; i < 16; i++) {
        payload = payload.append(mac_address);
    }
    return payload;
}

bool WakeOnLanManager::wake_up_host(const Host &host) {
    bool result = false;
    Data payload = create_payload(host);
    
    struct addrinfo hints, *res, *curr;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_ADDRCONFIG;
    
    if (getaddrinfo(host.address.c_str(), NULL, &hints, &res) != 0 || res == NULL) {
        // Failed to resolve address
        Logger::error("WakeOnLanManager", "Failed to resolve WOL address");
        return result;
    }
    
    // Try all addresses that this DNS name resolves to. We have
    // to create a new socket each time because the addresses
    // may be different address families.
    for (curr = res; curr != NULL; curr = curr->ai_next) {
        int wol_socket = socket(curr->ai_family, SOCK_DGRAM, IPPROTO_UDP);
        if (wol_socket < 0) {
            Logger::error("WakeOnLanManager", "Failed to create WOL socket");
            continue;
        }
        
        int val = 1;
        setsockopt(wol_socket, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
        
        struct sockaddr_storage addr;
        memset(&addr, 0, sizeof(addr));
        memcpy(&addr, curr->ai_addr, curr->ai_addrlen);
        
        for (int j = 0; j < numPorts; j++) {
            populate_address(&addr, ports[j]);
            long err = sendto(wol_socket, payload.bytes(), payload.size(), 0, (struct sockaddr*)&addr, curr->ai_addrlen);
            result = result || err > 0;
            Logger::info("WakeOnLanManager", "Sending WOL packet returned: %ld", err);
        }
        
        close(wol_socket);
    }
    freeaddrinfo(res);
    return result;
}
