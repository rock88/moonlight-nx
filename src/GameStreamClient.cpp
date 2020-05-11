#include "GameStreamClient.hpp"
#include "Settings.hpp"
#include <algorithm>
#include <vector>
#include <fstream>
#include <future>
#include <nanogui/nanogui.h>

#ifdef __SWITCH__
#include <switch.h>
#else
#include <thread>
#include <mutex>
#endif

#undef LOG
#define LOG printf("%s:%i\n", __FUNCTION__, __LINE__);

static std::mutex m_async_mutex;

#ifdef __SWITCH__
static void perform(void* context) {
    LOG
    auto task_ptr = static_cast<std::function<void()> *>(context);
    LOG
    (*task_ptr)();
    LOG
    delete task_ptr;
    threadExit();
    LOG
}
#endif

void perform_async(std::function<void()> task) {
    #ifdef __SWITCH__
    std::function<void()>* retained_task = new std::function<void()>(task);
    Thread thread;
    threadCreate(
        &thread,
        &perform,
        (void *)retained_task,
        NULL,
        0x10000,
        0x2C,
        -2
    );
    threadStart(&thread);
    #else
    auto thread = std::thread([task](){
        std::lock_guard<std::mutex> guard(m_async_mutex);
        task();
    });
    thread.detach();
    #endif
}

void GameStreamClient::connect(const std::string &address, ServerCallback<SERVER_DATA> callback) {
    LOG
    m_server_data[address] = SERVER_DATA();
    LOG
    perform_async([this, address, callback] {
        LOG
        // TODO: mem leak here :(
        std::string key_dir = Settings::settings()->working_dir() + "/key";
        LOG
        int status = gs_init(&m_server_data[address], (char *)(new std::string(address))->c_str(), key_dir.c_str(), 0, false);
        LOG
        nanogui::async([this, address, callback, status] {
            LOG
            if (status == GS_OK) {
                Settings::settings()->add_host(address);
                callback(GameStreamResult<SERVER_DATA>::success(m_server_data[address]));
            } else {
                callback(GameStreamResult<SERVER_DATA>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void GameStreamClient::pair(const std::string &address, const std::string &pin, ServerCallback<bool> callback) {
    if (m_server_data.count(address) == 0) {
        callback(GameStreamResult<bool>::failure("Firstly call connect()..."));
        return;
    }
    
    perform_async([this, address, pin, callback] {
        int status = gs_pair(&m_server_data[address], (char *)pin.c_str());
        
        nanogui::async([callback, status] {
            if (status == GS_OK) {
                callback(GameStreamResult<bool>::success(true));
            } else {
                callback(GameStreamResult<bool>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void GameStreamClient::applist(const std::string &address, ServerCallback<PAPP_LIST> callback) {
    if (m_server_data.count(address) == 0) {
        callback(GameStreamResult<PAPP_LIST>::failure("Firstly call connect() & pair()..."));
        return;
    }
    
    m_app_list[address] = PAPP_LIST();
    
    perform_async([this, address, callback] {
        int status = gs_applist(&m_server_data[address], &m_app_list[address]);
        
        nanogui::async([this, address, callback, status] {
            if (status == GS_OK) {
                callback(GameStreamResult<PAPP_LIST>::success(m_app_list[address]));
            } else {
                callback(GameStreamResult<PAPP_LIST>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void GameStreamClient::app_boxart(const std::string &address, int app_id, ServerCallback<std::pair<char*, size_t>> callback) {
    if (m_server_data.count(address) == 0) {
        callback(GameStreamResult<std::pair<char*, size_t>>::failure("Firstly call connect() & pair()..."));
        return;
    }
    
    perform_async([this, address, app_id, callback] {
        char* data;
        size_t size;
        
        int status = gs_app_boxart(&m_server_data[address], app_id, &data, &size);
        
        nanogui::async([this, callback, data, size, status] {
            if (status == GS_OK) {
                callback(GameStreamResult<std::pair<char*, size_t>>::success(std::make_pair(data, size)));
            } else {
                callback(GameStreamResult<std::pair<char*, size_t>>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void GameStreamClient::start(const std::string &address, STREAM_CONFIGURATION config, int app_id, ServerCallback<STREAM_CONFIGURATION> callback) {
    if (m_server_data.count(address) == 0) {
        callback(GameStreamResult<STREAM_CONFIGURATION>::failure("Firstly call connect() & pair()..."));
        return;
    }
    
    m_config = config;
    
    perform_async([this, address, app_id, callback] {
        int status = gs_start_app(&m_server_data[address], &m_config, app_id, false, false, 0);
        
        nanogui::async([this, callback, status] {
            if (status == GS_OK) {
                callback(GameStreamResult<STREAM_CONFIGURATION>::success(m_config));
            } else {
                callback(GameStreamResult<STREAM_CONFIGURATION>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}

void GameStreamClient::quit(const std::string &address, ServerCallback<bool> callback) {
    if (m_server_data.count(address) == 0) {
        callback(GameStreamResult<bool>::failure("Firstly call connect() & pair()..."));
        return;
    }
    
    perform_async([this, address, callback] {
        int status = gs_quit_app(&m_server_data[address]);
        
        nanogui::async([this, callback, status] {
            if (status == GS_OK) {
                callback(GameStreamResult<bool>::success(true));
            } else {
                callback(GameStreamResult<bool>::failure(gs_error != NULL ? gs_error : "Unknown error..."));
            }
        });
    });
}
