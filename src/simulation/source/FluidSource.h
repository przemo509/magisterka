#ifndef MAGISTERKA_FLUIDSOURCE_H
#define MAGISTERKA_FLUIDSOURCE_H


#include "../../utils/LinkedList.h"
#include "../../utils/Config.h"

class Config;

class FluidSource {
public:

    FluidSource(Config *config);

    int positionX;
    int positionY;
    int positionZ;
    int height;
    int radius;
    float spreadFactor;

    float densityFactor;

    int getStartX();

    int getEndX();

    int getStartY();

    int getEndY();

    int getStartZ();

    int getEndZ();

    float getCurrentVelocity();

    vector<float> parseVelocities(string velocitiesString);

    vector<int> parsePhaseFrames(string phaseFramesString);

    vector<int> parsePhaseVelocities(string phaseVelocitiesString);

    void calculateVelocitiesDistribution(vector<float> velocities, vector<int> phaseFrames, vector<int> phaseVelocities);

    float *velocitiesDistribution;

    virtual ~FluidSource();

    int maxFrameForVertices;
};


#endif //MAGISTERKA_FLUIDSOURCE_H
