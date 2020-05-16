#include <stdio.h>
#include <string>
#pragma once

class Data {
public:
    Data(): Data(0) {};
    Data(char* bytes, size_t size);
    Data(unsigned char* bytes, size_t size): Data((char *)bytes, size) {};
    Data(size_t capacity);
    
    ~Data();
    
    char* bytes() const {
        return m_bytes;
    }
    
    size_t size() const {
        return m_size;
    }
    
    Data subdata(size_t size);
    
    Data(const Data& that);
    Data& operator=(const Data& that);
    
    static Data random_bytes(size_t size);
    static Data read_from_file(std::string path);
    
    Data hex_to_bytes() const;
    Data bytes_to_hex() const;
    
    bool is_empty() const {
        return m_size == 0;
    }
    
    void write_to_file(std::string path);
    
private:
    char* m_bytes = nullptr;
    size_t m_size = 0;
};
