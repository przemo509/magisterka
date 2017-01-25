#ifndef MAGISTERKA_CONFIG_H
#define MAGISTERKA_CONFIG_H

#include "StringUtils.h"
#include "Logger.h"
#include "../simulation/source/FluidSource.h"

#include <iostream>
#include <fstream>

using namespace std;

class Config {
public:
    static const char numbersDelimiter = ';';

    int maxFrames;
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
    float thermalBuoyancyFactor;
    float gravityFactor;
    float windFactor;

    float sourceDensity;
    string sourceVelocitiesValuesString;
    string sourcePhaseFramesString;
    string sourcePhaseVelocitiesIdxsString;

    const int relaxationSteps = 12; // liczba iteracji relaksacji Gaussa-Seidela

    string configName;
    string dataDirectoryWithPrefix;
    string configFilePath;
    string configDescription;
    string blenderExecutablePath;
    string pythonScriptPath;
    string blenderScenePath;
    string ffmpegExecutablePath;

    static Config *getInstance() {
        return instance;
    }

    static void init(string configName) {
        instance = new Config(configName);
    }

private:
    const int requiredMainConfigValues = 20;
    const string mainConfigFilePath = ".\\config.txt";

    static Config *instance;

    Config(string configName);

    ~Config() {}

    Config(Config const &);

    void operator=(Config const &);

    void readConfig(bool isMain);

};


#endif //MAGISTERKA_CONFIG_H
