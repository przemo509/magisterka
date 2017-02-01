#ifndef MAGISTERKA_VORTEX_H
#define MAGISTERKA_VORTEX_H

#include "../../utils/LinkedListNode.h"
#include "../../utils/GeometryUtils.h"

class Vortex : public LinkedListNode {
public:
    Vortex(int id, int x, int y, int z, int domainSize);

    virtual ~Vortex();

    void apply(float *vx, float *vy, float *vz);

private:
    static const int vortexRemovalBorder = 5; // TODO zbadać rózne ustawienia
    const int domainSize;
    const int id;

    Point position;
    Vector direction; // rotacja + prędkość
    int radius;
    int framesLived;

    bool shouldBeRemoved();

    bool isBehindBorder(int border);

    friend class VerticesList;

    int I3D(int i, int j, int k) const;
};

#endif //MAGISTERKA_VORTEX_H
