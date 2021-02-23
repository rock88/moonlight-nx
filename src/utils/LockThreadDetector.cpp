#include "LockThreadDetector.hpp"
#include "Logger.hpp"

void* LockThreadDetector::entry(void* context) {
    while (true) {
        if (auto detector = static_cast<LockThreadDetector *>(context)) {
            if (detector->m_is_running) {
                if (detector->m_current != 0) {
                    std::lock_guard<std::mutex> lock(detector->m_mutex);
                    
                    if (detector->m_prev > detector->m_current) {
                        Logger::error("LockThreadDetector", "Thread Lock Detected on a line: %i", detector->m_last_line);
                    } else {
                        detector->m_prev++;
                    }
                }
                usleep(500'000);
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    }
    return NULL;
}

LockThreadDetector::LockThreadDetector(std::string file_name) {
    m_file_name = file_name;
}

void LockThreadDetector::start() {
    
}

void LockThreadDetector::stop() {
    
}

void LockThreadDetector::enter(int line) {
    
}
