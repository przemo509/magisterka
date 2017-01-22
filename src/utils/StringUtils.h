#ifndef MAGISTERKA_STRINGUTILS_H
#define MAGISTERKA_STRINGUTILS_H

#include <stdio.h>
#include <locale>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

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

static inline string intToString(int i, int width, char padding) {
    stringstream ss;
    ss << setfill(padding) << setw(width) << i << setfill(' ');
    return ss.str();
}

#endif //MAGISTERKA_STRINGUTILS_H
