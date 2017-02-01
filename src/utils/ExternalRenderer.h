#ifndef MAGISTERKA_EXTERNALRENDERER_H
#define MAGISTERKA_EXTERNALRENDERER_H

#include <string>
#include "../simulation/ExplosionSimulation.h"

typedef unsigned char BYTE;

using namespace std;

class ExternalRenderer {

public:
    ExternalRenderer(ExplosionSimulation *simulation);

    void renderFrame(int frame);

    void makeVideo(int i);

private:
    string dataDirectoryWithPrefix;
    ExplosionSimulation *simulation;
    int size;
    int cellsCount;

    void dumpDensity(string filePath);

    BYTE *composeOutArray();

    void runBlender(string densityFilePath, int i);

    void removeRenderedFrames(int frames);

    int I3D(int i, int j, int k);
};


#endif //MAGISTERKA_EXTERNALRENDERER_H
