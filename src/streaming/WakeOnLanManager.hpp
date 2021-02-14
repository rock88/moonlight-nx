#include "Singleton.hpp"
#include <stdio.h>

struct Host;

class WakeOnLanManager: public Singleton<WakeOnLanManager> {
private:
    bool wake_up_host(const Host &host);
    
    friend class GameStreamClient;
};
