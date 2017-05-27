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
    static const string BOOL_TRUE_STRING;

    int maxFrames;
    const int framesToSkipRender = 1; // ustawić na 1, żeby renderować wszystkie klatki

    const bool simulateVertices = true;
    float verticesCountFactor;
    float vortexStrength;
    float vortexMoving;
    int vortexRadiusMin;
    int vortexRadiusMax;
    int waveletTurbulenceAmplify;

    const int simulationSpaceSize = 100;
    const float dt = 0.25;

    float viscosity;
    float diffusionRate;
    float thermalBuoyancyFactor;
    float gravityFactor;
    float windFactor;
    bool solidVolumeBoundaries;

    string sourcePositionString;
    int sourceRadius;
    int sourceHeight;
    float sourceSpreadFactor;
    float sourceDensityFactor;
    string sourceVelocitiesValuesString;
    string sourcePhaseFramesString;
    string sourcePhaseVelocitiesIdxsString;

    const int relaxationSteps = 12; // liczba iteracji relaksacji Gaussa-Seidela

    int imgResX;
    int imgResY;
    int imgResPercentage;
    float rayTracingStepSize;
    int ffmpegCRF;

    const string mainConfigFilePath = "./config.txt";
    string configName;
    string dataDirectoryWithPrefix;
    string configFilePath;
    string configDescription;
    string blenderExecutablePath;
    string pythonScriptPath;
    string blenderScenePath;
    string ffmpegExecutablePath;
    string commandOnFinish;

    int saveSmallDensity;
    int saveBigDensity;
    int saveSmallFrames;
    int saveBigFrames;
    bool zipRawFiles;

    int threads;

    static Config *getInstance() {
        return instance;
    }

    static void init(string configName) {
        instance = new Config(configName);
    }

    bool useWaveletTurbulence() {
        return waveletTurbulenceAmplify > 0;
    }

    static bool featureEnabledAtFrame(int frame, int enabledFrames, int startingFrame);

private:
    const int requiredMainConfigValues = 38;

    static Config *instance;

    Config(string configName);

    ~Config() {}

    Config(Config const &);

    void operator=(Config const &);

    void readConfig(bool isMain);

};


#endif //MAGISTERKA_CONFIG_H
