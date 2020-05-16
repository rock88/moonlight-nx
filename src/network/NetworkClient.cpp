#include "NetworkClient.hpp"
#include "Settings.hpp"
#include "CryptoManager.hpp"
#include "errors.h"
#include "Log.h"
#include <curl/curl.h>

static CURL *curl;
static bool init_curl_completed = false;

struct HTTP_DATA {
    char *memory;
    size_t size;
};

static size_t _write_curl(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    HTTP_DATA* mem = (HTTP_DATA *)userp;
    
    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL)
        return 0;
    
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

static void init_curl() {
    if (init_curl_completed) {
        return;
    }
    
    std::string key_directory = Settings::settings()->working_dir() + "/key";
    
    curl = curl_easy_init();

    char certificateFilePath[4096];
    sprintf(certificateFilePath, "%s/%s", key_directory.c_str(), CERTIFICATE_FILE_NAME);

    char keyFilePath[4096];
    sprintf(&keyFilePath[0], "%s/%s", key_directory.c_str(), KEY_FILE_NAME);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, 1L);
    curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE,"PEM");
    curl_easy_setopt(curl, CURLOPT_SSLCERT, certificateFilePath);
    curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSLKEY, keyFilePath);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_curl);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, 0L);
    
    init_curl_completed = true;
}

int NetworkClient::perform(std::string url, bool use_https, Data* out) {
    init_curl();
    
    std::string _url;
    
    if (use_https) {
        _url = "https://" + url;
    } else {
        _url = "http://" + url;
    }
    
    LOG_FMT("Perform %s\n", _url.c_str());
    
    HTTP_DATA* http_data = (HTTP_DATA*)malloc(sizeof(HTTP_DATA));
    http_data->memory = (char*)malloc(1);
    http_data->size = 0;
    
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, http_data);
    curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
    
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        gs_error = curl_easy_strerror(res);
        return NetworkClientError;
    } else if (http_data->memory == NULL) {
        gs_error = "Out of memory";
        return NetworkClientError;
    }
    
    *out = Data(http_data->memory, (int)http_data->size);
    
    free(http_data->memory);
    free(http_data);
    
    return NetworkClientOK;
}
