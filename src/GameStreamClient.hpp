#include <string>
#include <vector>
#include <functional>
#include <map>

extern "C" {
    #include "client.h"
    #include "errors.h"
}

#pragma once

extern void perform_async(std::function<void()> task);

template <typename T>
struct GSResult {
public:
    static GSResult success(T value) {
        return result(value, "", true);
    }
    
    static GSResult failure(std::string error) {
        return result(T(), error, false);
    }
    
    bool isSuccess() const {
        return _isSuccess;
    }
    
    T value() const {
        return _value;
    }
    
    std::string error() const {
        return _error;
    }
    
private:
    static GSResult result(T value, std::string error, bool isSuccess) {
        GSResult result;
        result._value = value;
        result._error = error;
        result._isSuccess = isSuccess;
        return result;
    }
    
    T _value;
    std::string _error = "";
    bool _isSuccess = false;
};

template<class T> using ServerCallback = const std::function<void(GSResult<T>)>;

class GameStreamClient {
public:
    static GameStreamClient* client() {
        static GameStreamClient client;
        return &client;
    }
    
    SERVER_DATA server_data(const std::string &address) {
        return m_server_data[address];
    }
    
    void connect(const std::string &address, ServerCallback<SERVER_DATA> callback);
    void pair(const std::string &address, const std::string &pin, ServerCallback<bool> callback);
    void applist(const std::string &address, ServerCallback<PAPP_LIST> callback);
    void app_boxart(const std::string &address, int app_id, ServerCallback<std::pair<char*, size_t>> callback);
    void start(const std::string &address, STREAM_CONFIGURATION config, int app_id, ServerCallback<STREAM_CONFIGURATION> callback);
    void quit(const std::string &address, ServerCallback<bool> callback);
    
private:
    GameStreamClient();
    
    std::map<std::string, SERVER_DATA> m_server_data;
    std::map<std::string, PAPP_LIST> m_app_list;
    STREAM_CONFIGURATION m_config;
};
