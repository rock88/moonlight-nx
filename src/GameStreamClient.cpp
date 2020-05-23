#include "GameStreamClient.hpp"
#include "Settings.hpp"
#include <thread>
#include <mutex>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <future>
#include <nanogui/nanogui.h>
#include <unistd.h>

static std::mutex m_async_mutex;
static std::vector<std::function<void()>> m_tasks;

#ifdef __SWITCH__
#include <switch.h>
extern int moonlight_exit;

static void task_loop() {
    Thread thread;
    threadCreate(
        &thread,
        [](void* a) {
            while (!moonlight_exit) {
                std::vector<std::function<void()>> m_tasks_copy; {
                    std::lock_guard<std::mutex> guard(m_async_mutex);
                    m_tasks_copy = m_tasks;
                    m_tasks.clear();
                }

                for (auto task: m_tasks_copy) {
                    task();
                }

                usleep(500'000);
            }
        },
        NULL,
        NULL,
        0x10000,
        0x2C,
        -2
    );
    threadStart(&thread);
}
#else
static void task_loop() {
    auto thread = std::thread([](){
        while (1) {
            std::vector<std::function<void()>> m_tasks_copy; {
                std::lock_guard<std::mutex> guard(m_async_mutex);
                m_tasks_copy = m_tasks;
                m_tasks.clear();
            }
            
            for (auto task: m_tasks_copy) {
                task();
            }
            
            usleep(500'000);
        }
    });
    thread.detach();
}
#endif

void perform_async(std::function<void()> task) {
    std::lock_guard<std::mutex> guard(m_async_mutex);
    m_tasks.push_back(task);
}

GameStreamClient::GameStreamClient() {
    task_loop();
}

void GameStreamClient::connect(const std::string &address, ServerCallback<SERVER_DATA> callback) {
    m_server_data[address] = SERVER_DATA();
    
    perform_async([this, address, callback] {
        // TODO: mem leak here :(
        int status = gs_init(&m_server_data[address], (char *)(new std::string(address))->c_str(), Settings::settings()->key_dir().c_str(), 0, false);
        
        nanogui::async([this, address, callback, status] {
            if (status == GS_OK) {
                Settings::settings()->add_host(address);
                callback(GSResult<SERVER_DATA>::success(m_server_data[address]));
            } else {
                callback(GSResult<SERVER_DATA>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void GameStreamClient::pair(const std::string &address, const std::string &pin, ServerCallback<bool> callback) {
    if (m_server_data.count(address) == 0) {
        callback(GSResult<bool>::failure("Firstly call connect()..."));
        return;
    }
    
    perform_async([this, address, pin, callback] {
        int status = gs_pair(&m_server_data[address], (char *)pin.c_str());
        
        nanogui::async([callback, status] {
            if (status == GS_OK) {
                callback(GSResult<bool>::success(true));
            } else {
                callback(GSResult<bool>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void GameStreamClient::applist(const std::string &address, ServerCallback<PAPP_LIST> callback) {
    if (m_server_data.count(address) == 0) {
        callback(GSResult<PAPP_LIST>::failure("Firstly call connect() & pair()..."));
        return;
    }
    
    m_app_list[address] = PAPP_LIST();
    
    perform_async([this, address, callback] {
        int status = gs_applist(&m_server_data[address], &m_app_list[address]);
        
        nanogui::async([this, address, callback, status] {
            if (status == GS_OK) {
                callback(GSResult<PAPP_LIST>::success(m_app_list[address]));
            } else {
                callback(GSResult<PAPP_LIST>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void GameStreamClient::app_boxart(const std::string &address, int app_id, ServerCallback<Data> callback) {
    if (m_server_data.count(address) == 0) {
        callback(GSResult<Data>::failure("Firstly call connect() & pair()..."));
        return;
    }
    
    perform_async([this, address, app_id, callback] {
        Data data;
        int status = gs_app_boxart(&m_server_data[address], app_id, &data);
        
        nanogui::async([this, callback, data, status] {
            if (status == GS_OK) {
                callback(GSResult<Data>::success(data));
            } else {
                callback(GSResult<Data>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void GameStreamClient::start(const std::string &address, STREAM_CONFIGURATION config, int app_id, ServerCallback<STREAM_CONFIGURATION> callback) {
    if (m_server_data.count(address) == 0) {
        callback(GSResult<STREAM_CONFIGURATION>::failure("Firstly call connect() & pair()..."));
        return;
    }
    
    m_config = config;
    
    perform_async([this, address, app_id, callback] {
        int status = gs_start_app(&m_server_data[address], &m_config, app_id, false, false, 0);
        
        nanogui::async([this, callback, status] {
            if (status == GS_OK) {
                callback(GSResult<STREAM_CONFIGURATION>::success(m_config));
            } else {
                callback(GSResult<STREAM_CONFIGURATION>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void GameStreamClient::quit(const std::string &address, ServerCallback<bool> callback) {
    if (m_server_data.count(address) == 0) {
        callback(GSResult<bool>::failure("Firstly call connect() & pair()..."));
        return;
    }
    
    perform_async([this, address, callback] {
        int status = gs_quit_app(&m_server_data[address]);
        
        nanogui::async([this, callback, status] {
            if (status == GS_OK) {
                callback(GSResult<bool>::success(true));
            } else {
                callback(GSResult<bool>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}
