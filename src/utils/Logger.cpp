#include <iostream>
#include <cstdarg>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include "Logger.h"

extern "C"
{
#include "lib/snprintf/snprintf.h"
}

using namespace std;

Logger::Logger() {
}

Logger::~Logger() {
}

void Logger::info(string format, ...) {
    va_list args;
    va_start(args, format);
    log(cout, "INFO", format, args);
    va_end(args);
}

void Logger::warning(string format, ...) {
    va_list args;
    va_start(args, format);
    log(cout, "WARN", format, args);
    va_end(args);
}

void Logger::error(string format, ...) {
    va_list args;
    va_start(args, format);
    log(cerr, "ERR", format, args);
    va_end(args);
}

void Logger::debug1(string format, ...) {
    va_list args;
    va_start(args, format);
    debug(DEBUG_1, format, args);
    va_end(args);
}

void Logger::debug2(string format, ...) {
    va_list args;
    va_start(args, format);
    debug(DEBUG_2, format, args);
    va_end(args);
}

void Logger::debug3(string format, ...) {
    va_list args;
    va_start(args, format);
    debug(DEBUG_3, format, args);
    va_end(args);
}

void Logger::debug4(string format, ...) {
    va_list args;
    va_start(args, format);
    debug(DEBUG_4, format, args);
    va_end(args);
}

void Logger::debug5(string format, ...) {
    va_list args;
    va_start(args, format);
    debug(DEBUG_5, format, args);
    va_end(args);
}

void Logger::debug(DebugLevel lvl, string format, va_list args) {
    if(lvl <= currentLevel) {
        log(cout, "DBG_" + to_string(static_cast<int>(lvl)), format, args);
    }
}

string Logger::to_string(int i) {
    stringstream ss;
    ss << i;
    return ss.str();
}

void Logger::log(ostream &stream, string level, string format, va_list args) {
    string msg = formatMessage(format, args);

    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    stream << setfill('0')
           << setw(4) << (now->tm_year + 1900) << "-"
           << setw(2) << (now->tm_mon + 1) << "-"
           << setw(2) << now->tm_mday << " "
           << setw(2) << now->tm_hour << ":"
           << setw(2) << now->tm_min << ":"
           << setw(2) << now->tm_sec << " "
           << setfill(' ')
           << "[" << setw(5) << level << "] "
           << msg << endl;
}

string Logger::formatMessage(string format, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(NULL, 0, format.c_str(), args_copy) + 1;
    va_end(args_copy);

    char *text = (char *) malloc(len * sizeof(char));

    vsnprintf(text, len, format.c_str(), args);
    string msg(text);

    free(text);
    return msg;
}

void Logger::setCurrentLevel(DebugLevel currentLevel) {
    Logger::currentLevel = currentLevel;
}
