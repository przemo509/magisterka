#ifndef MAGISTERKA_EXPLOSIONSIMULATION_H
#define MAGISTERKA_EXPLOSIONSIMULATION_H

#include "../utils/GeometryUtils.h"
#include "Vortex.h"

enum BoundDirection {
    X_DIR, Y_DIR, Z_DIR, NO_DIR
};

class ExplosionSimulation {
public:
    ExplosionSimulation();

    virtual ~ExplosionSimulation();

    void proceed();
    vect3f getDensityArray() const;
    int getArraysSize() const;

    void setStartingConditions();

private:
    int N;
    vect3f vxPrev, vyPrev, vzPrev, vx, vy, vz;
    vect3f densPrev, dens;
    float dt, viscosity, diffusionRate;
    int relaxationSteps;

    Vortex **vertices;

    void calculateVelocities();

    void calculateDensities();

    void setBoundaries(BoundDirection dir, vect3f x);

    void project(vect3f u, vect3f v, vect3f w, vect3f p, vect3f div);

    void advect(BoundDirection dir, vect3f d, vect3f d0, vect3f u, vect3f v, vect3f w);

    void diffuse(BoundDirection dir, float factor, vect3f x, vect3f x0);

    void allocate3D(vect3f &what);

    void deallocate3D(vect3f &what);

    void addSources();

    void addForces();

    void addTurbulences();

    void clearSpace();

    void clearVerticesIfNeeded();

    void createNewVertices();

    friend class ExplosionVisualization;

};

#endif //MAGISTERKA_EXPLOSIONSIMULATION_H
