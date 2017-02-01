#include "VerticesList.h"
#include "../../utils/MathUtils.h"
#include "../../utils/Timer.h"

VerticesList::VerticesList(int domainSize, FluidSource *fluidSource) {
    this->domainSize = domainSize;
    this->fluidSource = fluidSource;
    calculateVerticesDistribution();
}

void VerticesList::calculateVerticesDistribution() {
    int frames = fluidSource->maxFrameForVertices;
    verticesDistribution = new int[frames];

    float reminder = 0.0;
    for (int frame = 0; frame < frames; ++frame) {
        float frameVerticesCountFloat = fluidSource->velocitiesDistribution[frame] * Config::getInstance()->verticesCountFactor + reminder;
        int frameVerticesCountInt = (int) frameVerticesCountFloat;
        reminder = frameVerticesCountFloat - frameVerticesCountInt;
        verticesDistribution[frame] = frameVerticesCountInt;
    }
}

VerticesList::~VerticesList() {
    delete[] verticesDistribution;
}

void VerticesList::applyAll(float *vx, float *vy, float *vz) {
    Vortex *tmp = (Vortex *) getFirst();
    while (tmp != NULL) {
        tmp->apply(vx, vy, vz);
        Vortex *next = (Vortex *) tmp->next;
        if (tmp->shouldBeRemoved()) {
            remove(tmp);
        }
        tmp = next;
    }
}

void VerticesList::addNewVertices() {
    int frame = Timer::getInstance().getCurrentFrame();
    int newVerticesCount = verticesDistribution[frame];
    for (int i = 0; i < newVerticesCount; i++) {
        addNewVortex();
    }
}

void VerticesList::addNewVortex() {
    int vortexInitX = randAround(fluidSource->positionX, 2);
    int vortexInitZ = randAround(fluidSource->positionZ, 2);
    Vortex *vortex = new Vortex(getCurrentCount(), vortexInitX, vortexInitY, vortexInitZ, domainSize);
    add(vortex);
}
