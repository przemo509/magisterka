//////////////////////////////////////////////////////////////////////
// This file is part of Wavelet Turbulence.
//
// Wavelet Turbulence is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Wavelet Turbulence is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Wavelet Turbulence.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2008 Theodore Kim and Nils Thuerey
//
// FLUID_3D.h: interface for the FLUID_3D class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FLUID_3D_H
#define FLUID_3D_H

#include <cstdlib>
#include <cmath>
#include <iostream>
#include "OBSTACLE.h"
#include "VEC3.h"

using namespace std;
using namespace BasicVector;

class WTURBULENCE;

class FLUID_3D {
public:
    FLUID_3D(int xRes, int yRes, int zRes, int amplify);

    FLUID_3D() {};

    virtual ~FLUID_3D();

    // create & allocate vector noise advection
    void initVectorNoise(int amplify);

    void addSmokeColumn();

    static void addSmokeTestCase(float *field, Vec3Int res);

    void step();

    void addObstacle(OBSTACLE *obstacle);

    const float *xVelocity() { return _xVelocity; };

    const float *yVelocity() { return _yVelocity; };

    const float *zVelocity() { return _zVelocity; };

    int xRes() const { return _xRes; };

    int yRes() const { return _yRes; };

    int zRes() const { return _zRes; };

protected:
    // dimensions
    int _xRes, _yRes, _zRes, _maxRes;
    Vec3Int _res;
    int _totalCells;
    int _slabSize;
    float _dx;
    float _totalTime;
    int _totalSteps;
    int _totalImgDumps;
    int _totalVelDumps;

    void artificialDamping(float *field);

    // fields
    float *_density;
    float *_densityOld;
    float *_heat;
    float *_heatOld;
    float *_pressure;
    float *_xVelocity;
    float *_yVelocity;
    float *_zVelocity;
    float *_xVelocityOld;
    float *_yVelocityOld;
    float *_zVelocityOld;
    float *_xForce;
    float *_yForce;
    float *_zForce;
    float *_divergence;
    float *_xVorticity;
    float *_yVorticity;
    float *_zVorticity;
    float *_vorticity;
    unsigned char *_obstacles;

    // CG fields
    float *_residual;
    float *_direction;
    float *_q;
    int _iterations;

    // simulation constants
    float _dt;
    float _buoyancy;
    float _vorticityEps;
    float _heatDiffusion;

    // WTURBULENCE object, if active
    WTURBULENCE *_wTurbulence;

    // boundary setting functions
    void copyBorderAll(float *field);

    // timestepping functions
    void wipeBoundaries();

    void addForce();

    void addVorticity();

    void addBuoyancy(float *field);

    // solver stuff
    void project();

    void diffuseHeat();

    void solvePressure(float *field, float *b, unsigned char *skip);

    void solveHeat(float *field, float *b, unsigned char *skip);

    // handle obstacle boundaries
    void setObstacleBoundaries();

public:
    // advection, accessed e.g. by WTURBULENCE class
    void advectMacCormack();

    // boundary setting functions
    static void copyBorderX(float *field, Vec3Int res);

    static void copyBorderY(float *field, Vec3Int res);

    static void copyBorderZ(float *field, Vec3Int res);

    static void setNeumannX(float *field, Vec3Int res);

    static void setNeumannY(float *field, Vec3Int res);

    static void setNeumannZ(float *field, Vec3Int res);

    static void setZeroX(float *field, Vec3Int res);

    static void setZeroY(float *field, Vec3Int res);

    static void setZeroZ(float *field, Vec3Int res);

    static void setZeroBorder(float *field, Vec3Int res) {
        setZeroX(field, res);
        setZeroY(field, res);
        setZeroZ(field, res);
    };

    // static advection functions, also used by WTURBULENCE
    static void advectFieldSemiLagrange(const float dt, const float *velx, const float *vely, const float *velz,
                                        float *oldField, float *newField, Vec3Int res);

    static void advectFieldMacCormack(const float dt, const float *xVelocity, const float *yVelocity, const float *zVelocity,
                                      float *oldField, float *newField, float *temp1, float *temp2, Vec3Int res, const float *obstacles);

    // maccormack helper functions
    static void clampExtrema(const float dt, const float *xVelocity, const float *yVelocity, const float *zVelocity,
                             float *oldField, float *newField, Vec3Int res);

    static void clampOutsideRays(const float dt, const float *xVelocity, const float *yVelocity, const float *zVelocity,
                                 float *oldField, float *newField, Vec3Int res, const float *obstacles, const float *oldAdvection);
};

#endif

