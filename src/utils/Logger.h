#ifndef MAGISTERKA_LOGGER_H
#define MAGISTERKA_LOGGER_H

#include <string>
#include <vector>

class Logger {
public:

    static Logger &getInstance() {
        static Logger instance;
        return instance;
    }

    void printOnScreen();

    void addLineToScreen(int lineNumber, const char *format, ...);

private:
    Logger();

    Logger(Logger const &);

    void operator=(Logger const &);

    virtual ~Logger();

    std::vector<std::string> guiTextLines;
};

#endif //MAGISTERKA_LOGGER_H
