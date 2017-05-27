#ifndef MAGISTERKA_STRINGUTILS_H
#define MAGISTERKA_STRINGUTILS_H

#include <stdio.h>
#include <locale>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

static const int secInMin = 60;
static const int minInHour = 60;
static const int hourInDay = 24;
static const int secInHour = secInMin * minInHour;
static const int secInDay = secInHour * hourInDay;

static vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


static vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

static inline string &ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
    return s;
}

static inline string &rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

static inline string &trim(string &s) {
    return ltrim(rtrim(s));
}

static inline string intToString(int i) {
    stringstream ss;
    ss << i;
    return ss.str();
}

static inline string floatToString(float f) {
    stringstream ss;
    ss << f;
    return ss.str();
}

static inline string intToString(int i, int width, char padding) {
    stringstream ss;
    ss << setfill(padding) << setw(width) << i << setfill(' ');
    return ss.str();
}

static string formatTime(int seconds) {
    int d = seconds / secInDay;
    int h = (seconds % secInDay) / secInHour;
    int m = (seconds % secInHour) / secInMin;
    int s = seconds % secInMin;
    return (d > 0 ? intToString(d) + "d " : "") + intToString(h, 2, '0') + ":" + intToString(m, 2, '0') + ":" + intToString(s, 2, '0');
}

#endif //MAGISTERKA_STRINGUTILS_H
