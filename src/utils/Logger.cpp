#include <iostream>
#include <cstdarg>
#include "Logger.h"

using namespace std;

Logger::Logger() {
    guiTextLines.assign(25, "");
}

Logger::~Logger() {
}

void Logger::printOnScreen() {
    int lineNumber = 1;
    for (vector<string>::iterator v_i = guiTextLines.begin(); v_i != guiTextLines.end(); ++v_i, ++lineNumber) {
        string line = *v_i;
        cout << line;
    }


}

void Logger::addLineToScreen(int lineNumber, const char *format, ...) {
    va_list args;
    int len;
    char *text;

    va_start(args, format);
    len = vsnprintf(NULL, 0, format, args) + 1;
    text = (char *) malloc(len * sizeof(char));
    vsnprintf(text, len, format, args);

    va_end(args);

    guiTextLines[lineNumber] = text;

    free(text);
}
