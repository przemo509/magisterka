#ifndef MAGISTERKA_EXPLOSIONSIMULATION_H
#define MAGISTERKA_EXPLOSIONSIMULATION_H

#include "../utils/GeometryUtils.h"
#include "vortex/Vortex.h"
#include "vortex/VerticesList.h"
#include "source/FluidSource.h"
#include "wavelet/WTURBULENCE.h"

enum BoundDirection {
    X_DIR, Y_DIR, Z_DIR, NO_DIR
};

class ExplosionSimulation {
public:
    ExplosionSimulation();

    virtual ~ExplosionSimulation();

    void proceed();

    float *getDensityArray();

    static int getArraysSize();

    static int getArraysSize3D();

    void setStartingConditions();

    static int I3D(int i, int j, int k);

private:
    static int N;
    float *vxPrev, *vyPrev, *vzPrev, *vx, *vy, *vz;
    float *densPrev, *dens;
    float dt, viscosity, diffusionRate;
    int relaxationSteps;

    FluidSource *source;
    VerticesList *vertices;

    void calculateVelocities();

    void calculateDensities();

    void setBoundaries(BoundDirection dir, float *x);

    void project(float *u, float *v, float *w, float *p, float *div);

    void advect(BoundDirection dir, float *d, float *d0, float *u, float *v, float *w);

    void diffuse(BoundDirection dir, float factor, float *x, float *x0);

    void addSources();

    void addForces();

    void addTurbulences();

    void clearSpace();

    friend class ExternalRenderer;

    unsigned char *obstacles;

    WTURBULENCE *waveletTurbulence;
};

#endif //MAGISTERKA_EXPLOSIONSIMULATION_H
