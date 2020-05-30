#include <stdio.h>
#include <stdarg.h>
#pragma once

class Logger {
public:
    static void debug(const char* tag, const char *format, ...);
    static void info(const char* tag, const char *format, ...);
    static void warning(const char* tag, const char *format, ...);
    static void error(const char* tag, const char *format, ...);
    static void fatal(const char* tag, const char *format, ...);
    
private:
    static void log(const char* level, const char* tag, const char *format, va_list list);
};
