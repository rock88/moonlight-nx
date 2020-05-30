#include "Logger.hpp"
#include "Settings.hpp"
#include <algorithm>
#include <string>
#include <mutex>
#include <time.h>

static std::mutex m_log_mutex;

void Logger::debug(const char* tag, const char *format, ...) {
    va_list list;
    va_start(list, format);
    log("DEBUG", tag, format, list);
    va_end(list);
}

void Logger::info(const char* tag, const char *format, ...) {
    va_list list;
    va_start(list, format);
    log("INFO", tag, format, list);
    va_end(list);
}

void Logger::warning(const char* tag, const char *format, ...) {
    va_list list;
    va_start(list, format);
    log("WARNING", tag, format, list);
    va_end(list);
}

void Logger::error(const char* tag, const char *format, ...) {
    va_list list;
    va_start(list, format);
    log("ERROR", tag, format, list);
    va_end(list);
}

void Logger::fatal(const char* tag, const char *format, ...) {
    va_list list;
    va_start(list, format);
    log("FATAL", tag, format, list);
    va_end(list);
}

void Logger::log(const char* level, const char* tag, const char *format, va_list list) {
    if (!Settings::settings()->write_log()) {
        return;
    }
    
    std::lock_guard<std::mutex> guard(m_log_mutex);
    
    char buf[4096];
    vsnprintf(buf, sizeof(buf), format, list);
    
    time_t unix_time = time(NULL);
    struct tm* time_struct = localtime((const time_t *)&unix_time);
    
    char time_buf[32];
    sprintf(time_buf, "%i-%.2i-%.2i %.2i:%.2i:%.2i",
            time_struct->tm_year + 1900,
            time_struct->tm_mon,
            time_struct->tm_wday,
            time_struct->tm_hour,
            time_struct->tm_min,
            time_struct->tm_sec
            );
    
    std::string tag_str = std::string(tag);
    for (auto & c: tag_str) c = toupper(c);
    
    std::string str = std::string(time_buf) + " [" + std::string(level) + "]["  + std::string(tag_str) + "] " + std::string(buf) + "\n";
    
    printf("%s", str.c_str());
    
    FILE *log = fopen(Settings::settings()->log_path().c_str(), "a");
    if (log) {
        fwrite(str.c_str(), str.length(), 1, log);
        fclose(log);
    }
}
