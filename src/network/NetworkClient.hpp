#include "Data.hpp"
#include <string>
#pragma once

#define NetworkClientOK 0
#define NetworkClientError 1

static const std::string unique_id = "0123456789ABCDEF";

class NetworkClient {
public:
    static int perform(std::string url, bool use_https, Data* out);
};
