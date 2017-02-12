#include "Vortex.h"
#include "../../utils/MathUtils.h"
#include "../../utils/Config.h"
#include "../ExplosionSimulation.h"

Vortex::Vortex(int id, int x, int y, int z, int domainSize) : id(id), domainSize(domainSize) {
    position = Point(x, y, z);
    direction = Vector(rand(0, 50), rand(0, 50), rand(0, 50));
    radius = rand(Config::getInstance()->vortexRadiusMin, Config::getInstance()->vortexRadiusMax);
    framesLived = 0;
}

Vortex::~Vortex() {
}

void Vortex::apply(float *vx, float *vy, float *vz, float *dens) {
    for (int k = (int) (position.z - radius); k <= position.z + radius; ++k) {
        if (k < 0) continue;
        if (k >= domainSize) break;
        for (int j = (int) (position.y - radius); j <= position.y + radius; ++j) {
            if (j < 0) continue;
            if (j >= domainSize) break;
            for (int i = (int) (position.x - radius); i <= position.x + radius; ++i) {
                if (i < 0) continue;
                if (i >= domainSize) break;
                Vector windDirection = Vector(position, Point(i, j, k)).crossProduct(direction);
                windDirection.normalize();
                int idx3D = I3D(i, j, k);
                vx[idx3D] += Config::getInstance()->vortexStrength / radius * windDirection.x;
                vy[idx3D] += Config::getInstance()->vortexStrength / radius * windDirection.y;
                vz[idx3D] += Config::getInstance()->vortexStrength / radius * windDirection.z;
            }
        }
    }

    int oldX = (int) position.x;
    int oldY = (int) position.y;
    int oldZ = (int) position.z;
    int oldIdx3D = I3D(oldX, oldY, oldZ);
    position.x += Config::getInstance()->vortexMoving * vx[oldIdx3D] * dens[oldIdx3D];
    position.y += Config::getInstance()->vortexMoving * vy[oldIdx3D] * dens[oldIdx3D];
    position.z += Config::getInstance()->vortexMoving * vz[oldIdx3D] * dens[oldIdx3D];
    framesLived++;
}

int Vortex::I3D(int i, int j, int k) const {
    return ExplosionSimulation::I3D(i, j, k, domainSize);
}

bool Vortex::shouldBeRemoved() {
    return isBehindBorder(0) || (framesLived > 10 && isBehindBorder(vortexRemovalBorder)); // TODO zbadać różne ustawienia
}

bool Vortex::isBehindBorder(int border) {
    return position.x <= border || position.x >= domainSize - border ||
           position.y <= border || position.y >= domainSize - border ||
           position.z <= border || position.z >= domainSize - border;
}
