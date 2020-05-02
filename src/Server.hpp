#include <string>
#include <vector>

extern "C" {
    #include "client.h"
    #include "errors.h"
}

#pragma once

template <typename T>
struct Result {
public:
    static Result success(T* value) {
        return result(value, "", true);
    }
    
    static Result failure(std::string error) {
        return result(NULL, error, false);
    }
    
    bool isSuccess() const {
        return _isSuccess;
    }
    
    T* value() const {
        return _value;
    }
    
    std::string error() const {
        return _error;
    }
    
private:
    static Result result(T* value, std::string error, bool isSuccess) {
        Result result;
        result._value = value;
        result._error = error;
        result._isSuccess = isSuccess;
        return result;
    }
    
    T* _value;
    std::string _error = "";
    bool _isSuccess = false;
};

template<class T> using ServerCallback = const std::function<void(Result<T>)>;

class Server {
public:
    static Server* server() {
        static Server server;
        return &server;
    }

    void set_working_dir(const std::string &dir) {
        m_working_dir = std::string(dir + "/moonlight");
    }
    
    void add_host(std::string address);
    std::vector<std::string> hosts();
    
    void connect(std::string address, ServerCallback<SERVER_DATA> &callback);

private:
    Server();
    
    std::string m_working_dir;
    std::vector<std::string> m_hosts;
};
