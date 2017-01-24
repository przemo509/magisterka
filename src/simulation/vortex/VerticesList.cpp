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
    float velocitySum = 0.0;
    for (int frame = 0; frame < frames; ++frame) {
        velocitySum += fluidSource->velocitiesDistribution[frame];
    }
    float verticesPerVelocityUnit = Config::getInstance()->verticesCount / velocitySum; // TODO czy to się dobrze podzieli? czy rzutować na float?

    float reminder = 0.0;
    for (int frame = 0; frame < frames; ++frame) {
        float frameVerticesCountFloat = fluidSource->velocitiesDistribution[frame] * verticesPerVelocityUnit + reminder;
        int frameVerticesCountInt = (int) frameVerticesCountFloat;
        reminder = frameVerticesCountFloat - frameVerticesCountInt;
        verticesDistribution[frame] = frameVerticesCountInt;
    }
}

VerticesList::~VerticesList() {
    delete[] verticesDistribution;
}

void VerticesList::applyAll(vect3f vx, vect3f vy, vect3f vz) {
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
        addNewVertex();
    }
}

void VerticesList::addNewVertex() {
    int vortexInitX = randAround(fluidSource->positionX, 2);
    int vortexInitZ = randAround(fluidSource->positionZ, 2);
    Vortex *vertex = new Vortex(vortexInitX, vortexInitY, vortexInitZ, domainSize);
    add(vertex);
}
