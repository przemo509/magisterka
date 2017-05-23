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

    void makeVideo(int frames);

private:
    string dataDirectoryWithPrefix;
    ExplosionSimulation *simulation;

    void dumpDensity(string filePath, float *density, int size);

    BYTE *composeOutArray(float *density, int size);

    void runBlender(string densityFilePath, string outputFilePrefix, int frame, int size);

    void makeVideo(int frames, string outputFilePrefix, int saveFrames);

    void removeRenderedFrames(int frames, string outputFilePrefix, int saveFrames);

    int I3D(int i, int j, int k, int cubeSize);

    bool shouldRemove(int frame, int saveFrames);

    void zipDensityFile(string basic_string);

    void renderFrame(int frame, string densityFilePrefix, int saveFrames, float *densityArray, int arraySize);
};


#endif //MAGISTERKA_EXTERNALRENDERER_H
