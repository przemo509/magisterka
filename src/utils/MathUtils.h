//
// Created by ubuntu on 11/19/16.
//

#ifndef MAGISTERKA_MATHUTILS_H
#define MAGISTERKA_MATHUTILS_H


#include <cstdlib>

static int rand(int from, int to) {
    return rand() % (to - from + 1) + from;
}

static int randAround(int middle, int spread) {
    return middle + rand(-spread, spread);
}

static float randFloat(float from, float to) {
    return 1.0f * rand() / RAND_MAX * (to - from) + from;
}

static float randAroundFloat(float middle, float spread) {
    return middle + randFloat(-spread, spread);
}

#endif //MAGISTERKA_MATHUTILS_H
