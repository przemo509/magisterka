#include "Vortex.h"
#include "../../utils/MathUtils.h"
#include "../../utils/Config.h"

Vortex::Vortex(int x, int y, int z, int domainSize) : domainSize(domainSize) {
    position = Point(x, y, z);
    direction = Vector(rand(0, 50), rand(0, 50), rand(0, 50));
    radius = rand(Config::getInstance()->vortexRadiusMin, Config::getInstance()->vortexRadiusMax);
    framesLived = 0;
}

Vortex::~Vortex() {
}

void Vortex::apply(vect3f vx, vect3f vy, vect3f vz) {
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
                vx[i][j][k] += Config::getInstance()->vortexStrength / radius * windDirection.x;
                vy[i][j][k] += Config::getInstance()->vortexStrength / radius * windDirection.y;
                vz[i][j][k] += Config::getInstance()->vortexStrength / radius * windDirection.z;
            }
        }
    }

    position.x += Config::getInstance()->vortexMoving * vx[(int) position.x][(int) position.y][(int) position.z];
    position.y += Config::getInstance()->vortexMoving * vy[(int) position.x][(int) position.y][(int) position.z];
    position.z += Config::getInstance()->vortexMoving * vz[(int) position.x][(int) position.y][(int) position.z];
    framesLived++;
}

bool Vortex::shouldBeRemoved() {
    return framesLived > 10 && isBehindRemovalBorder(); // TODO zbadać różne ustawienia
}

bool Vortex::isBehindRemovalBorder() {
    return position.x <= vortexRemovalBorder || position.x >= domainSize - vortexRemovalBorder ||
           position.y <= vortexRemovalBorder || position.y >= domainSize - vortexRemovalBorder ||
           position.z <= vortexRemovalBorder || position.z >= domainSize - vortexRemovalBorder;
}
