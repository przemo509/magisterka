#include "ExplosionSimulation.h"
#include "../utils/Config.h"
#include "../utils/MathUtils.h"
#include "../utils/Timer.h"

vect3f ExplosionSimulation::getDensityArray() const {
    return dens;
}

int ExplosionSimulation::getArraysSize() const {
    return N + 2;
}

ExplosionSimulation::ExplosionSimulation() {
    N = Config::getInstance()->simulationSpaceSize;

    viscosity = Config::getInstance()->viscosity;
    diffusionRate = Config::getInstance()->diffusionRate;

    dt = Config::getInstance()->dt;

    relaxationSteps = Config::getInstance()->relaxationSteps;

    allocate3D(vx);
    allocate3D(vy);
    allocate3D(vz);
    allocate3D(vxPrev);
    allocate3D(vyPrev);
    allocate3D(vzPrev);
    allocate3D(dens);
    allocate3D(densPrev);

    vertices = new Vortex *[Config::getInstance()->verticesCount];
    for (int i = 0; i < Config::getInstance()->verticesCount; ++i) {
        vertices[i] = NULL;
    }

    setStartingConditions();
}

void ExplosionSimulation::allocate3D(vect3f &t) {
    int size = getArraysSize();
    t = new float **[size];
    for (int i = 0; i < size; ++i) {
        t[i] = new float *[size];
        for (int j = 0; j < size; ++j) {
            t[i][j] = new float[size];
        }
    }
}

ExplosionSimulation::~ExplosionSimulation() {
    deallocate3D(vx);
    deallocate3D(vy);
    deallocate3D(vz);
    deallocate3D(vxPrev);
    deallocate3D(vyPrev);
    deallocate3D(vzPrev);
    deallocate3D(dens);
    deallocate3D(densPrev);
}

void ExplosionSimulation::deallocate3D(vect3f &t) {
    int size = getArraysSize();
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            delete[] t[i][j];
        }
        delete[] t[i];
    }
    delete[] t;
}

void ExplosionSimulation::clearVerticesIfNeeded() {
    for (int i = 0; i < Config::getInstance()->verticesCount; ++i) {
        if (vertices[i] != NULL) {
            delete vertices[i];
        }
    }
}

void ExplosionSimulation::setStartingConditions() {
    clearSpace();
    clearVerticesIfNeeded();
    createNewVertices();
}

void ExplosionSimulation::clearSpace() {
    int size = getArraysSize();
    for (int k = 0; k < size; ++k) {
        for (int j = 0; j < size; ++j) {
            for (int i = 0; i < size; ++i) {
                dens[i][j][k] = 0.0;
                vx[i][j][k] = 0.0;
                vy[i][j][k] = 0.0;
                vz[i][j][k] = 0.0;
                densPrev[i][j][k] = 0.0;
                vxPrev[i][j][k] = 0.0;
                vyPrev[i][j][k] = 0.0;
                vzPrev[i][j][k] = 0.0;
            }
        }
    }
}

void ExplosionSimulation::createNewVertices() {
    int vortexInitY = 10;
    int verticesStartsFromFrame = 10;
    int verticesStartsToFrame = Config::getInstance()->simulationLengthFrames;
    float vortexEveryFrame =
            (float) ((verticesStartsToFrame - verticesStartsFromFrame)) / Config::getInstance()->verticesCount;
    for (int v = 0; v < Config::getInstance()->verticesCount; ++v) {
        vertices[v] = new Vortex(randAround(Config::getInstance()->mainSourceCenterX, 2), vortexInitY,
                                 randAround(Config::getInstance()->mainSourceCenterZ, 2),
                                 v * vortexEveryFrame);
    }
}

void ExplosionSimulation::proceed() {
    addSources();
    addForces();
    addTurbulences();
    calculateVelocities();
    calculateDensities();
}

void ExplosionSimulation::addSources() {
    const int currentFrame = Timer::getInstance().getCurrentFrame();

    int height = Config::getInstance()->mainSourceHeight;
    int radius = Config::getInstance()->mainSourceRadius;
    int centerX = Config::getInstance()->mainSourceCenterX;
    int centerZ = Config::getInstance()->mainSourceCenterZ;

    float phaseVelocity = 0.0;
    for (int nextPhase = 1; nextPhase < Config::getInstance()->explosionSourcePhases; ++nextPhase) {
        int nextPhaseStartFrame = Config::getInstance()->explosionSource[nextPhase][0];
        if (nextPhaseStartFrame > currentFrame) {
            phaseVelocity = Config::getInstance()->explosionSource[nextPhase - 1][1];
            break;
        }

    }

    for (int k = centerZ - radius; k < centerZ + radius; ++k) {
        for (int j = 2; j < height + 2; ++j) {
            for (int i = centerX - radius; i < centerX + radius; ++i) {
                dens[i][j][k] += dt / (sqrt((i - centerX) * (i - centerX) + (k - centerZ) * (k - centerZ)) + 0.01) *
                                 Config::getInstance()->mainSourceDensity;
                vx[i][j][k] += dt * sin(i - centerX) * Config::getInstance()->mainSourceSpreadFactor; // na zewnątrz
                vz[i][j][k] += dt * sin(k - centerZ) * Config::getInstance()->mainSourceSpreadFactor; // na zewnątrz
                vy[i][j][k] += dt * phaseVelocity;
            }
        }
    }
}

void ExplosionSimulation::addForces() {
    int size = getArraysSize();
    for (int k = 0; k < size; ++k) {
        for (int j = 0; j < size; ++j) {
            for (int i = 0; i < size; ++i) {
                vy[i][j][k] += dt * (-1.0 * Config::getInstance()->gravityFactor +
                                     dens[i][j][k] * Config::getInstance()->thermalBuoyancyFactor / (j > 0 ? j : 1));
                if (i < 5) {
                    vx[i][j][k] += dt * Config::getInstance()->windFactor;
                }
            }
        }
    }
}

void ExplosionSimulation::addTurbulences() {
    if (!Config::getInstance()->simulateTurbulences) {
        return;
    }

    for (int i = 0; i < Config::getInstance()->verticesCount; ++i) {
        vertices[i]->apply(vx, vy, vz, getArraysSize());
    }
}

void ExplosionSimulation::calculateVelocities() {
    std::swap(vxPrev, vx);
    std::swap(vyPrev, vy);
    std::swap(vzPrev, vz);

    diffuse(X_DIR, viscosity, vx, vxPrev);
    diffuse(Y_DIR, viscosity, vy, vyPrev);
    diffuse(Z_DIR, viscosity, vz, vzPrev);

    project(vx, vy, vz, vxPrev, vyPrev);

    std::swap(vxPrev, vx);
    std::swap(vyPrev, vy);
    std::swap(vzPrev, vz);

    advect(X_DIR, vx, vxPrev, vxPrev, vyPrev, vzPrev);
    advect(Y_DIR, vy, vyPrev, vxPrev, vyPrev, vzPrev);
    advect(Z_DIR, vz, vzPrev, vxPrev, vyPrev, vzPrev);
    project(vx, vy, vz, vxPrev, vyPrev);
}

void ExplosionSimulation::calculateDensities() {
    std::swap(densPrev, dens);
    diffuse(NO_DIR, diffusionRate, dens, densPrev);
    std::swap(densPrev, dens);
    advect(NO_DIR, dens, densPrev, vx, vy, vz);
}

/**
 * Niestabilna dyfuzja.
 */
//void ExplosionSimulation::diffuse(BoundDirection dir, float factor, vect3f x, vect3f x0) {
//	float a = dt * factor * N * N;
//	for(int k = 1; k <= N; ++k) {
//		for(int j = 1; j <= N; ++j) {
//			for(int i = 1; i <= N; ++i) {
//				x[i][j][k] =
//						x0[i][j][k] +
//						a*(
//						x0[i-1][j][k] +
//						x0[i+1][j][k] +
//						x0[i][j-1][k] +
//						x0[i][j+1][k] +
//						x0[i][j][k-1] +
//						x0[i][j][k+1] -
//						6 * x0[i][j][k]
//						);
//			}
//		}
//	}
//	setBoundaries(dir, x);
//}

/**
 * stabilna dyfuzja.
 */
void ExplosionSimulation::diffuse(BoundDirection dir, float factor, vect3f x, vect3f x0) {
    float a = dt * factor * N * N;
    float precomputedDivider = 1.0 + 6.0 * a;
    for (int step = 0; step < relaxationSteps; ++step) {
        for (int k = 1; k <= N; ++k) {
            for (int j = 1; j <= N; j++) {
                for (int i = 1; i <= N; i++) {
                    x[i][j][k] = (x0[i][j][k] + a * (x[i - 1][j][k] + x[i + 1][j][k] + x[i][j - 1][k] + x[i][j + 1][k] +
                                                     x[i][j][k - 1] + x[i][j][k + 1]))
                                 / precomputedDivider;
                }
            }
        }
        setBoundaries(dir, x);
    }
}

void ExplosionSimulation::setBoundaries(BoundDirection dir, vect3f x) {
    int xSign = dir == X_DIR ? -1 : 1;
    int ySign = dir == Y_DIR ? -1 : 1;
    int zSign = dir == Z_DIR ? -1 : 1;

    // ściany (6)
    for (int i = 1; i <= N; ++i) {
        for (int j = 1; j <= N; ++j) {
            // przednia
            x[i][j][0] = zSign * x[i][j][1];
            // tylna
            x[i][j][N + 1] = zSign * x[i][j][N];
            // lewa
            x[0][i][j] = xSign * x[1][i][j];
            // prawa
            x[N + 1][i][j] = xSign * x[N][i][j];
            // dolna
            x[i][0][j] = ySign * x[i][1][j];
            // górna
            x[i][N + 1][j] = ySign * x[i][N][j];
        }
    }

    // krawędzie (12)
    for (int i = 1; i <= N; ++i) {
        // pionowo
        x[0][i][0] = (x[1][i][0] + x[0][i][1]) / 2;
        x[N + 1][i][0] = (x[N][i][0] + x[N + 1][i][1]) / 2;
        x[0][i][N + 1] = (x[1][i][N + 1] + x[0][i][N]) / 2;
        x[N + 1][i][N + 1] = (x[N][i][N + 1] + x[N + 1][i][N]) / 2;

        // wszerz
        x[i][0][0] = (x[i][1][0] + x[i][0][1]) / 2;
        x[i][N + 1][0] = (x[i][N][0] + x[i][N + 1][1]) / 2;
        x[i][0][N + 1] = (x[i][1][N + 1] + x[i][0][N]) / 2;
        x[i][N + 1][N + 1] = (x[i][N][N + 1] + x[i][N + 1][N]) / 2;

        // wgłąb
        x[0][0][i] = (x[1][0][i] + x[0][1][i]) / 2;
        x[N + 1][0][i] = (x[N][0][i] + x[N + 1][1][i]) / 2;
        x[0][N + 1][i] = (x[1][N + 1][i] + x[0][N][i]) / 2;
        x[N + 1][N + 1][i] = (x[N][N + 1][i] + x[N + 1][N][i]) / 2;
    }

    // narożniki (8)
    x[0][0][0] = (x[1][0][0] + x[0][1][0] + x[0][0][1]) / 3;
    x[0][0][N + 1] = (x[1][0][N + 1] + x[0][1][N + 1] + x[0][0][N]) / 3;
    x[0][N + 1][0] = (x[1][N + 1][0] + x[0][N][0] + x[0][N + 1][1]) / 3;
    x[0][N + 1][N + 1] = (x[1][N + 1][N + 1] + x[0][N][N + 1] + x[0][N + 1][N]) / 3;
    x[N + 1][0][0] = (x[N][0][0] + x[N + 1][1][0] + x[N + 1][0][1]) / 3;
    x[N + 1][0][N + 1] = (x[N][0][N + 1] + x[N + 1][1][N + 1] + x[N + 1][0][N]) / 3;
    x[N + 1][N + 1][0] = (x[N][N + 1][0] + x[N + 1][N][0] + x[N + 1][N + 1][1]) / 3;
    x[N + 1][N + 1][N + 1] = (x[N][N + 1][N + 1] + x[N + 1][N][N + 1] + x[N + 1][N + 1][N]) / 3;
}

void ExplosionSimulation::project(vect3f u, vect3f v, vect3f w, vect3f p, vect3f div) {
    for (int k = 1; k <= N; ++k) {
        for (int j = 1; j <= N; ++j) {
            for (int i = 1; i <= N; ++i) {
                div[i][j][k] = -(u[i + 1][j][k] - u[i - 1][j][k] + v[i][j + 1][k] - v[i][j - 1][k] + w[i][j][k + 1] -
                                 w[i][j][k - 1]) / 3.0 / N;
                p[i][j][k] = 0.0;
            }
        }
    }
    setBoundaries(NO_DIR, div);
    setBoundaries(NO_DIR, p);

    for (int step = 0; step < relaxationSteps; ++step) {
        for (int k = 1; k <= N; ++k) {
            for (int j = 1; j <= N; ++j) {
                for (int i = 1; i <= N; ++i) {
                    p[i][j][k] = (div[i][j][k] + p[i - 1][j][k] + p[i + 1][j][k] + p[i][j - 1][k] + p[i][j + 1][k] +
                                  p[i][j][k - 1] + p[i][j][k + 1]) / 6.0;
                }
            }
        }
        setBoundaries(NO_DIR, p);
    }

    for (int k = 1; k <= N; ++k) {
        for (int j = 1; j <= N; ++j) {
            for (int i = 1; i <= N; ++i) {
                u[i][j][k] -= (p[i + 1][j][k] - p[i - 1][j][k]) / 2.0 * N;
                v[i][j][k] -= (p[i][j + 1][k] - p[i][j - 1][k]) / 2.0 * N;
                w[i][j][k] -= (p[i][j][k + 1] - p[i][j][k - 1]) / 2.0 * N;
            }
        }
    }
    setBoundaries(X_DIR, u);
    setBoundaries(Y_DIR, v);
    setBoundaries(Z_DIR, w);
}

void ExplosionSimulation::advect(BoundDirection dir, vect3f d, vect3f d0, vect3f u, vect3f v, vect3f w) {
    for (int k = 1; k <= N; ++k) {
        for (int j = 1; j <= N; ++j) {
            for (int i = 1; i <= N; ++i) {
                // i, j, k - współrzędne komórki, dla której liczymy nowe dane
                // x, y, z - współrzędne punktu, z którego przyszły dane idąc wstecz po wektorze prędkości (nie pokrywa się z żadną komórką 1:1)
                float x = i - dt * N * u[i][j][k];
                float y = j - dt * N * v[i][j][k];
                float z = k - dt * N * w[i][j][k];

                // pilnujemy przekraczania granic podczas szukania
                if (x < 0.5)
                    x = 0.5;
                if (x > N + 0.5)
                    x = N + 0.5;
                if (y < 0.5)
                    y = 0.5;
                if (y > N + 0.5)
                    y = N + 0.5;
                if (z < 0.5)
                    z = 0.5;
                if (z > N + 0.5)
                    z = N + 0.5;

                // współrzędne ośmiu najbliższych komórek (kostka 2x2x2)
                int i0 = (int) x;
                int i1 = i0 + 1;
                int j0 = (int) y;
                int j1 = j0 + 1;
                int k0 = (int) z;
                int k1 = k0 + 1;

                // współczynniki z jakimi bierzemy dane z poszczególnych komórek (zależne od odległości od ich środków)
                float r1 = x - i0;
                float r0 = 1 - r1;
                float s1 = y - j0;
                float s0 = 1 - s1;
                float t1 = z - k0;
                float t0 = 1 - t1;

                // obliczenie wynikowej wartości w komórce [i][j][k]
                d[i][j][k] =
                        r0 * s0 * t0 * d0[i0][j0][k0] +
                        r0 * s0 * t1 * d0[i0][j0][k1] +
                        r0 * s1 * t0 * d0[i0][j1][k0] +
                        r0 * s1 * t1 * d0[i0][j1][k1] +
                        r1 * s0 * t0 * d0[i1][j0][k0] +
                        r1 * s0 * t1 * d0[i1][j0][k1] +
                        r1 * s1 * t0 * d0[i1][j1][k0] +
                        r1 * s1 * t1 * d0[i1][j1][k1];

            }
        }
    }
    setBoundaries(dir, d);
}
