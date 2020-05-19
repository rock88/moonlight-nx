#include <netinet/in.h>
#include <stdint.h>
#include <switch.h>

uint32_t htonl(uint32_t hostlong) {
    return __builtin_bswap32(hostlong);
}

uint16_t htons(uint16_t hostshort) {
    return __builtin_bswap16(hostshort);
}

uint32_t ntohl(uint32_t netlong) {
    return __builtin_bswap32(netlong);
}

uint16_t ntohs(uint16_t netshort) {
    return __builtin_bswap16(netshort);
}

int sigaction(int a, const struct sigaction* b, struct sigaction* c) {
    return 0;
}

static int nxlink_sock = -1;

void userAppInit() {
    socketInitializeDefault();
    nxlinkStdio();
}

void userAppExit() {
    socketExit();
}
