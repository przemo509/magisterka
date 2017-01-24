#ifndef MAGISTERKA_VERTICESLIST_H
#define MAGISTERKA_VERTICESLIST_H

#include "Vortex.h"
#include "../../utils/LinkedList.h"
#include "../../utils/GeometryUtils.h"
#include "../source/FluidSource.h"


class VerticesList : public LinkedList {
public:
    VerticesList(int domainSize, FluidSource *source);

    void applyAll(vect3f vx, vect3f vy, vect3f vz);

    void addNewVertices();

    virtual ~VerticesList();

private:
    static const int vortexInitY = 10;
    int domainSize;
    int *verticesDistribution;
    FluidSource *fluidSource;

    void addNewVortex();

    void calculateVerticesDistribution();
};


#endif //MAGISTERKA_VERTICESLIST_H
