#include <stdio.h>
#include <mutex>
#include <switch.h>
#pragma once

class LockThreadDetector {
    LockThreadDetector(std::string file_name);
    
    void start();
    void stop();
    
    void enter(int line);
    
private:
    static void* entry(void* context);
    
    std::string m_file_name;
    bool m_is_running = false;
    u64 m_prev = 0;
    u64 m_current = 0;
    int m_last_line = 0;
    
    std::mutex m_mutex;
    pthread_t m_thread;
};
