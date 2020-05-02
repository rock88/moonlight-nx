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

using Task = const function<void()>;

mutex m_task_mutex;
vector<Task> m_tasks;
thread m_task_thread;

void perform_load_task(Task &task) {
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

void Server::connect(string address, ServerCallback<SERVER_DATA> &callback) {
    perform_load_task([this, address, callback] {
        SERVER_DATA data;
        
        int status = gs_init(&data, (char *)address.c_str(), (m_working_dir + "/key").c_str(), 0, false);
        
        nanogui::async([address, callback, data, status] {
            SERVER_DATA copy(data);
            
            if (status == GS_OK) {
                callback(Result<SERVER_DATA>::success(&copy));
                //free(data);
            } else {
                if (gs_error != NULL) {
                    callback(Result<SERVER_DATA>::failure(gs_error));
                } else {
                    callback(Result<SERVER_DATA>::failure("Unknown error..."));
                }
            }
        });
    });
}
