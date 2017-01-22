#include <iostream>
#include <cstdarg>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <cstring>
#include "Logger.h"
#include "StringUtils.h"

extern "C"
{
#include "lib/snprintf/snprintf.h"
}

using namespace std;

Logger *Logger::instance;

Logger::Logger(string dataDirectoryWithPrefix) {
    logFilePath = dataDirectoryWithPrefix + "_log.txt";
    ofstream logFile(logFilePath, ios::out);
    if (!logFile.is_open()) {
        cerr << "Blad otwarcia pliku [" + logFilePath + "]. Szczegoly:\nkod=[" + intToString(errno) + "]\nkomunikat=[" + strerror(errno) + "]\n\nKoncze prace!";
        exit(EXIT_FAILURE);
    }
    logFile.close();
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
    if (lvl <= currentLevel) {
        log(cout, "DBG_" + intToString(static_cast<int>(lvl)), format, args);
    }
}

void Logger::log(ostream &stream, string level, string format, va_list args) {
    string msg = formatMessage(format, args);

    time_t t = time(NULL);
    struct tm *now = localtime(&t);

    stringstream ss;
    ss << setfill('0')
       << setw(4) << (now->tm_year + 1900) << "-"
       << setw(2) << (now->tm_mon + 1) << "-"
       << setw(2) << now->tm_mday << " "
       << setw(2) << now->tm_hour << ":"
       << setw(2) << now->tm_min << ":"
       << setw(2) << now->tm_sec << " "
       << setfill(' ')
       << "[" << setw(5) << level << "] "
       << msg << endl;
    string str = ss.str();
    stream << str;

    ofstream logFile(logFilePath, ios::app);
    if (!logFile.is_open()) {
        cerr << "Blad otwarcia pliku [" + logFilePath + "]. Szczegoly:\nkod=[" + intToString(errno) + "]\nkomunikat=[" + strerror(errno) + "]\n\nKoncze prace!";
        exit(EXIT_FAILURE);
    }
    logFile << str;
    logFile.close();
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
