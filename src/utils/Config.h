#ifndef MAGISTERKA_CONFIG_H
#define MAGISTERKA_CONFIG_H

#include "StringUtils.h"
#include "Logger.h"

#include <iostream>
#include <fstream>

using namespace std;

class Config {
public:
    int maxFrames;
    int simulationLengthFrames = 400;
    const int framesToSkipRender = 1; // 1 żeby renderować wszystkie klatki

    const bool simulateTurbulences = true;
    int verticesCount;
    float vortexStrength;
    int vortexMoving;
    int vortexRadiusMin;
    int vortexRadiusMax;

    const int simulationSpaceSize = 100;

    const float dt = 0.25;
    float viscosity;
    float diffusionRate;

    const int mainSourceHeight = 3;
    const int mainSourceRadius = 6;
    const int mainSourceCenterX = 0 + mainSourceRadius + 5; // po lewej stronie, trochę dalej od krawędzi;
    const int mainSourceCenterZ = simulationSpaceSize / 2;
    float mainSourceDensity;
    const float mainSourceSpreadFactor = 0.35;
    float vySlow = 0;
    float vyMedium = 0;
    float vyFast = 0;
    float mainSourceVY = 0;
    float explosionSource[10][2];
    int explosionSourcePhases;

    float thermalBuoyancyFactor;

    float gravityFactor;
    float windFactor;

    const int relaxationSteps = 12; // liczba iteracji relaksacji Gaussa-Seidela

    string configName;
    string exportDirectory;

    static Config *getInstance() {
        return instance;
    }

    static void init(string configFileName) {
        instance = new Config(configFileName);
    }

private:
    static Config *instance;

    Config(string configFileName);

    ~Config() {}

    Config(Config const &);

    void operator=(Config const &);


};


#endif //MAGISTERKA_CONFIG_H
