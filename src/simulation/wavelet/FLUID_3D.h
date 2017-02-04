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
#include "VEC3.h"

using namespace BasicVector;

class FLUID_3D {
public:
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

