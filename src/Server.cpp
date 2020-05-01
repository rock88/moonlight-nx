#include "Server.hpp"
#include <thread>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <vector>
#include <nanogui/nanogui.h>

using Task = const std::function<void()>;

std::mutex m_task_mutex;
std::vector<Task> m_tasks;
std::thread m_task_thread;

void perform_load_task(Task &task) {
    std::lock_guard<std::mutex> guard(m_task_mutex);
    m_tasks.push_back(task);
}

Server::Server() {
    auto delay = std::chrono::microseconds((int64_t)(500'000));
    
    m_task_thread = std::thread([delay]() {
            while (true) {
                std::vector<Task> tasks; {
                    std::lock_guard<std::mutex> guard(m_task_mutex);
                    tasks = std::vector(m_tasks);
                    m_tasks.clear();
                }
                
                for (auto task: tasks) {
                    task();
                }
                std::this_thread::sleep_for(delay);
            }
        }
    );
}

void Server::connect(std::string address, ServerCallback<SERVER_DATA> &callback) {
    perform_load_task([address, callback] {
        SERVER_DATA data;
        
        int status = gs_init(&data, (char *)address.c_str(), "/Users/rock88/Documents/Projects/RetroArch/moonlight-libretro/key", 0, false);
        
        nanogui::async([address, callback, &data, status] {
            if (status == GS_OK) {
                callback(Result<SERVER_DATA>::success(&data));
            } else {
                callback(Result<SERVER_DATA>::failure(gs_error));
            }
        });
    });
}
