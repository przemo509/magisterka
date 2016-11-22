#ifndef MAGISTERKA_LOGGER_H
#define MAGISTERKA_LOGGER_H

#include <string>
#include <vector>

using namespace std;

enum DebugLevel {
    DEBUG_OFF, DEBUG_1, DEBUG_2, DEBUG_3, DEBUG_4, DEBUG_5
};

class Logger {
public:

    static Logger &getInstance() {
        static Logger instance;
        return instance;
    }

    void error(string format, ...);

    void info(string format, ...);

    void debug1(string format, ...);

    void debug2(string format, ...);

    void debug3(string format, ...);

    void debug4(string format, ...);

    void debug5(string format, ...);

    void warning(string format, ...);

private:
    DebugLevel currentLevel = DEBUG_OFF;
public:
    void setCurrentLevel(DebugLevel currentLevel);

private:
    void log(ostream &stream, string level, string format, va_list args);

    void debug(DebugLevel lvl, string format, va_list args);

    string formatMessage(string format, va_list args);

    Logger();

    Logger(Logger const &);

    void operator=(Logger const &);

    virtual ~Logger();

};

#endif //MAGISTERKA_LOGGER_H
