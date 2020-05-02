#include "Server.hpp"
#include <thread>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <nanogui/nanogui.h>

using namespace std;

using Task = function<void()>;

mutex m_task_mutex;
vector<Task> m_tasks;
thread m_task_thread;

void perform_load_task(Task task) {
    lock_guard<std::mutex> guard(m_task_mutex);
    m_tasks.push_back(task);
}

Server::Server() {
    auto delay = chrono::microseconds((int64_t)(500'000));
    
    m_task_thread = thread([delay]() {
            while (true) {
                vector<Task> tasks; {
                    lock_guard<mutex> guard(m_task_mutex);
                    tasks = vector(m_tasks);
                    m_tasks.clear();
                }
                
                for (auto task: tasks) {
                    task();
                }
                this_thread::sleep_for(delay);
            }
        }
    );
}

void Server::add_host(string address) {
    if (find(m_hosts.begin(), m_hosts.end(), address) == m_hosts.end()) {
        m_hosts.push_back(address);
    }
    
    ofstream file(m_working_dir + "/hosts.txt");
    
    for (auto host: m_hosts) {
        file << host << endl;
    }
    
    file.close();
}

vector<string> Server::hosts() {
    if (!m_hosts.empty()) {
        return m_hosts;
    }
    
    ifstream in(m_working_dir + "/hosts.txt");
    
    if (!in) {
        return {};
    }
    
    string str;
    while (getline(in, str)) {
        if (str.size() > 0)
            m_hosts.push_back(str);
    }
    in.close();
    return m_hosts;
}

void Server::connect(const string &address, ServerCallback(SERVER_DATA) callback) {
    perform_load_task([this, address, callback] {
        SERVER_DATA data;
        
        int status = gs_init(&data, (char *)address.c_str(), (m_working_dir + "/key").c_str(), 0, false);
        
        nanogui::async([address, callback, data, status] {
            if (status == GS_OK) {
                callback(Result<SERVER_DATA>::success(data));
            } else {
                callback(Result<SERVER_DATA>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void Server::pair(SERVER_DATA data, const string &pin, ServerCallback(bool) callback) {
    perform_load_task([data, pin, callback] {
        SERVER_DATA copy(data);
        
        int status = gs_pair(&copy, (char *)pin.c_str());
        
        nanogui::async([callback, status] {
            if (status == GS_OK) {
                callback(Result<bool>::success(true));
            } else {
                callback(Result<bool>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void Server::applist(SERVER_DATA data, ServerCallback(PAPP_LIST) callback) {
    perform_load_task([data, callback] {
        SERVER_DATA copy(data);
        PAPP_LIST app_list;
        
        int status = gs_applist(&copy, &app_list);
        
        nanogui::async([callback, app_list, status] {
            if (status == GS_OK) {
                callback(Result<PAPP_LIST>::success(app_list));
            } else {
                callback(Result<PAPP_LIST>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void Server::start(SERVER_DATA data, STREAM_CONFIGURATION config, int appId, ServerCallback(STREAM_CONFIGURATION) callback) {
    perform_load_task([data, config, appId, callback] {
        SERVER_DATA copy_data(data);
        STREAM_CONFIGURATION copy_config(config);
        
        int status = gs_start_app(&copy_data, &copy_config, appId, false, false, 0);
        
        nanogui::async([callback, status, copy_config] {
            if (status == GS_OK) {
                callback(Result<STREAM_CONFIGURATION>::success(copy_config));
            } else {
                callback(Result<STREAM_CONFIGURATION>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}
