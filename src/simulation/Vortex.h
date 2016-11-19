#ifndef MAGISTERKA_VORTEX_H
#define MAGISTERKA_VORTEX_H

#include "../utils/GeometryUtils.h"

class Vortex {
public:
    Vortex(int x, int y, int z, int startFrame);

    virtual ~Vortex();

    bool isActive();

    void apply(vect3f vx, vect3f vy, vect3f vz, int N);

private:
    Point position;
    Point startingPosition;
    Vector direction; // rotacja + prędkość
    int radius;
    int startFrame;
    int lifeFrames;

    friend class ExplosionVisualization;
};


#endif //MAGISTERKA_VORTEX_H
