#include <stdio.h>

struct Host;

class WakeOnLanManager {
private:
    static WakeOnLanManager* manager() {
        static WakeOnLanManager manager;
        return &manager;
    }
    
    bool wake_up_host(const Host &host);
    
    friend class GameStreamClient;
};
