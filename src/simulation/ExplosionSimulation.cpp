#include "ExplosionSimulation.h"
#include "../utils/Config.h"
#include "../utils/Timer.h"
#include "wavelet/IMAGE.h"
#include "wavelet/OBSTACLE.h"

float *ExplosionSimulation::getDensityArray() {
    return dens;
}


int ExplosionSimulation::N = 0;

int ExplosionSimulation::getArraysSize() {
    return N + 2;
}

int ExplosionSimulation::getArraysSize3D() {
    return getArraysSize() * getArraysSize() * getArraysSize();
}

ExplosionSimulation::ExplosionSimulation() {
    N = Config::getInstance()->simulationSpaceSize;

    viscosity = Config::getInstance()->viscosity;
    diffusionRate = Config::getInstance()->diffusionRate;

    dt = Config::getInstance()->dt;

    relaxationSteps = Config::getInstance()->relaxationSteps;

    int size3D = getArraysSize3D();
    vx = new float[size3D];
    vy = new float[size3D];
    vz = new float[size3D];
    vxPrev = new float[size3D];
    vyPrev = new float[size3D];
    vzPrev = new float[size3D];
    dens = new float[size3D];
    densPrev = new float[size3D];
    obstacles = new unsigned char[size3D];

    source = new FluidSource(Config::getInstance());
    vertices = new VerticesList(getArraysSize(), source);

    if (Config::getInstance()->useWaveletTurbulence) {
        waveletTurbulence = new WTURBULENCE(getArraysSize(), getArraysSize(), getArraysSize(), Config::getInstance()->waveletTurbulenceAmplify);
    } else {
        waveletTurbulence = NULL;
    }

    setStartingConditions();
}

ExplosionSimulation::~ExplosionSimulation() {
    delete[] vx;
    delete[] vy;
    delete[] vz;
    delete[] vxPrev;
    delete[] vyPrev;
    delete[] vzPrev;
    delete[] dens;
    delete[] densPrev;
    delete[] obstacles;
    delete source;
    delete vertices;
    if (waveletTurbulence) {
        delete waveletTurbulence;
    }
}

void ExplosionSimulation::setStartingConditions() {
    clearSpace();
}

void ExplosionSimulation::clearSpace() {
    int size3D = getArraysSize3D();
    for (int i = 0; i < size3D; ++i) {
        dens[i] = 0.0;
        vx[i] = 0.0;
        vy[i] = 0.0;
        vz[i] = 0.0;
        densPrev[i] = 0.0;
        vxPrev[i] = 0.0;
        vyPrev[i] = 0.0;
        vzPrev[i] = 0.0;
        obstacles[i] = EMPTY;
    }
}

void ExplosionSimulation::proceed() {
    addSources();
    addForces();
    addTurbulences();
    calculateVelocities();
    calculateDensities();

    if (waveletTurbulence) {
        float dx = 1.0f / getArraysSize();
        waveletTurbulence->stepTurbulenceFull(dt / dx, vx, vy, vz, obstacles);
    }
}

void ExplosionSimulation::addSources() {
    int cubeSize = getArraysSize();
    int wCubeSize = waveletTurbulence ? waveletTurbulence->getResBig().x : -1;
    int startX = source->getStartX();
    int endX = source->getEndX();
    int startY = source->getStartY();
    int endY = source->getEndY();
    int endZ = source->getEndZ();
    int startZ = source->getStartZ();
    float dvy = dt * source->getCurrentVelocity(); // bieżąca prędkość źródła w górę

    // pętla po wszystkich komórkach źródła (które nie jest punktowe, tylko ma swoją objetość)
    for (int k = startZ; k < endZ; ++k) {
        int dk = k - source->positionZ;
        double dvz = dt * sin(dk) * source->spreadFactor; // na zewnątrz
        for (int j = startY; j < endY; ++j) {
            for (int i = startX; i < endX; ++i) {
                int di = i - source->positionX;
                double dd = dt / (sqrt(di * di + dk * dk) + 0.01) * source->density;
                double dvx = dt * sin(di) * source->spreadFactor;  // na zewnątrz
                int idx3D = I3D(i, j, k, cubeSize);
                dens[idx3D] += dd;
                vx[idx3D] += dvx;
                vz[idx3D] += dvz;
                vy[idx3D] += dvy;

                if(waveletTurbulence) {
                    int amplify = Config::getInstance()->waveletTurbulenceAmplify;
                    for (int wk = k * amplify; wk < (k + 1) * amplify; wk++) {
                        for (int wj = j * amplify; wj < (j + 1) * amplify; wj++) {
                            for (int wi = i * amplify; wi < (i + 1) * amplify; wi++) {
                                int wIdx3D = I3D(wi, wj, wk, wCubeSize);
                                waveletTurbulence->getDensityBig()[wIdx3D] += dd;
                            }
                        }
                    }
                }
            }
        }
    }
}

void ExplosionSimulation::addForces() {
    int size = getArraysSize();
    for (int k = 0; k < size; ++k) {
        for (int j = 0; j < size; ++j) {
            for (int i = 0; i < size; ++i) {
                int idx3D = I3D(i, j, k, N);
                vy[idx3D] += dt * (-1.0 * Config::getInstance()->gravityFactor +
                                   dens[idx3D] * Config::getInstance()->thermalBuoyancyFactor / (j > 0 ? j : 1));
                if (i < 5) {
                    vx[idx3D] += dt * Config::getInstance()->windFactor;
                }
            }
        }
    }
}

void ExplosionSimulation::addTurbulences() {
    if (!Config::getInstance()->simulateTurbulences) {
        return;
    }

    vertices->addNewVertices();
    vertices->applyAll(vx, vy, vz);
}

void ExplosionSimulation::calculateVelocities() {
    SWAP_POINTERS(vxPrev, vx);
    SWAP_POINTERS(vyPrev, vy);
    SWAP_POINTERS(vzPrev, vz);

    diffuse(X_DIR, viscosity, vx, vxPrev);
    diffuse(Y_DIR, viscosity, vy, vyPrev);
    diffuse(Z_DIR, viscosity, vz, vzPrev);

    project(vx, vy, vz, vxPrev, vyPrev);

    SWAP_POINTERS(vxPrev, vx);
    SWAP_POINTERS(vyPrev, vy);
    SWAP_POINTERS(vzPrev, vz);

    advect(X_DIR, vx, vxPrev, vxPrev, vyPrev, vzPrev, N, dt);
    advect(Y_DIR, vy, vyPrev, vxPrev, vyPrev, vzPrev, N, dt);
    advect(Z_DIR, vz, vzPrev, vxPrev, vyPrev, vzPrev, N, dt);
    project(vx, vy, vz, vxPrev, vyPrev);
}

void ExplosionSimulation::calculateDensities() {
    SWAP_POINTERS(densPrev, dens);
    diffuse(NO_DIR, diffusionRate, dens, densPrev);
    SWAP_POINTERS(densPrev, dens);
    advect(NO_DIR, dens, densPrev, vx, vy, vz, N, dt);
}

/**
 * Niestabilna dyfuzja.
 */
//void ExplosionSimulation::diffuse(BoundDirection dir, float factor, vect3f x, vect3f x0) {
//	float a = dt * factor * N * N;
//	for(int k = 1; k <= N; ++k) {
//		for(int j = 1; j <= N; ++j) {
//			for(int i = 1; i <= N; ++i) {
//				x[I3D(i, j, k, cubeSize)] =
//						x0[I3D(i, j, k, cubeSize)] +
//						a*(
//						x0[I3D(i-1, j, k, cubeSize)] +
//						x0[I3D(i+1, j, k, cubeSize)] +
//						x0[I3D(i, j-1, k, cubeSize)] +
//						x0[I3D(i, j+1, k, cubeSize)] +
//						x0[I3D(i, j, k-1, cubeSize)] +
//						x0[I3D(i, j, k+1, cubeSize)] -
//						6 * x0[I3D(i, j, k, cubeSize)]
//						);
//			}
//		}
//	}
//	setBoundaries(dir, x);
//}

/**
 * stabilna dyfuzja.
 */
void ExplosionSimulation::diffuse(BoundDirection dir, float factor, float *x, float *x0) {
    int cubeSize = getArraysSize();
    float a = dt * factor * N * N;
    float precomputedDivider = 1.0f + 6.0f * a;
    for (int step = 0; step < relaxationSteps; ++step) {
        for (int k = 1; k <= N; ++k) {
            for (int j = 1; j <= N; j++) {
                for (int i = 1; i <= N; i++) {
                    x[I3D(i, j, k, cubeSize)] =
                            (x0[I3D(i, j, k, cubeSize)] + a * (x[I3D(i - 1, j, k, cubeSize)] + x[I3D(i + 1, j, k, cubeSize)] +
                                                               x[I3D(i, j - 1, k, cubeSize)] + x[I3D(i, j + 1, k, cubeSize)] +
                                                               x[I3D(i, j, k - 1, cubeSize)] + x[I3D(i, j, k + 1, cubeSize)])) / precomputedDivider;
                }
            }
        }
        setBoundaries(dir, x, N);
    }
}

void ExplosionSimulation::setBoundaries(BoundDirection dir, float *x, int N) {
    int cubeSize = N + 2;
    int xSign = dir == X_DIR ? -1 : 1;
    int ySign = dir == Y_DIR ? -1 : 1;
    int zSign = dir == Z_DIR ? -1 : 1;

    // ściany (6)
    for (int i = 1; i <= N; ++i) {
        for (int j = 1; j <= N; ++j) {
            // przednia
            x[I3D(i, j, 0, cubeSize)] = zSign * x[I3D(i, j, 1, cubeSize)];
            // tylna
            x[I3D(i, j, N + 1, cubeSize)] = zSign * x[I3D(i, j, N, cubeSize)];
            // lewa
            x[I3D(0, i, j, cubeSize)] = xSign * x[I3D(1, i, j, cubeSize)];
            // prawa
            x[I3D(N + 1, i, j, cubeSize)] = xSign * x[I3D(N, i, j, cubeSize)];
            // dolna
            x[I3D(i, 0, j, cubeSize)] = ySign * x[I3D(i, 1, j, cubeSize)];
            // górna
            x[I3D(i, N + 1, j, cubeSize)] = ySign * x[I3D(i, N, j, cubeSize)];
        }
    }

    // krawędzie (12)
    for (int i = 1; i <= N; ++i) {
        // pionowo
        x[I3D(0, i, 0, cubeSize)] = (x[I3D(1, i, 0, cubeSize)] + x[I3D(0, i, 1, cubeSize)]) / 2;
        x[I3D(N + 1, i, 0, cubeSize)] = (x[I3D(N, i, 0, cubeSize)] + x[I3D(N + 1, i, 1, cubeSize)]) / 2;
        x[I3D(0, i, N + 1, cubeSize)] = (x[I3D(1, i, N + 1, cubeSize)] + x[I3D(0, i, N, cubeSize)]) / 2;
        x[I3D(N + 1, i, N + 1, cubeSize)] = (x[I3D(N, i, N + 1, cubeSize)] + x[I3D(N + 1, i, N, cubeSize)]) / 2;

        // wszerz
        x[I3D(i, 0, 0, cubeSize)] = (x[I3D(i, 1, 0, cubeSize)] + x[I3D(i, 0, 1, cubeSize)]) / 2;
        x[I3D(i, N + 1, 0, cubeSize)] = (x[I3D(i, N, 0, cubeSize)] + x[I3D(i, N + 1, 1, cubeSize)]) / 2;
        x[I3D(i, 0, N + 1, cubeSize)] = (x[I3D(i, 1, N + 1, cubeSize)] + x[I3D(i, 0, N, cubeSize)]) / 2;
        x[I3D(i, N + 1, N + 1, cubeSize)] = (x[I3D(i, N, N + 1, cubeSize)] + x[I3D(i, N + 1, N, cubeSize)]) / 2;

        // wgłąb
        x[I3D(0, 0, i, cubeSize)] = (x[I3D(1, 0, i, cubeSize)] + x[I3D(0, 1, i, cubeSize)]) / 2;
        x[I3D(N + 1, 0, i, cubeSize)] = (x[I3D(N, 0, i, cubeSize)] + x[I3D(N + 1, 1, i, cubeSize)]) / 2;
        x[I3D(0, N + 1, i, cubeSize)] = (x[I3D(1, N + 1, i, cubeSize)] + x[I3D(0, N, i, cubeSize)]) / 2;
        x[I3D(N + 1, N + 1, i, cubeSize)] = (x[I3D(N, N + 1, i, cubeSize)] + x[I3D(N + 1, N, i, cubeSize)]) / 2;
    }

    // narożniki (8)
    x[I3D(0, 0, 0, cubeSize)] = (x[I3D(1, 0, 0, cubeSize)] + x[I3D(0, 1, 0, cubeSize)] + x[I3D(0, 0, 1, cubeSize)]) / 3;
    x[I3D(0, 0, N + 1, cubeSize)] = (x[I3D(1, 0, N + 1, cubeSize)] + x[I3D(0, 1, N + 1, cubeSize)] + x[I3D(0, 0, N, cubeSize)]) / 3;
    x[I3D(0, N + 1, 0, cubeSize)] = (x[I3D(1, N + 1, 0, cubeSize)] + x[I3D(0, N, 0, cubeSize)] + x[I3D(0, N + 1, 1, cubeSize)]) / 3;
    x[I3D(0, N + 1, N + 1, cubeSize)] = (x[I3D(1, N + 1, N + 1, cubeSize)] + x[I3D(0, N, N + 1, cubeSize)] + x[I3D(0, N + 1, N, cubeSize)]) / 3;
    x[I3D(N + 1, 0, 0, cubeSize)] = (x[I3D(N, 0, 0, cubeSize)] + x[I3D(N + 1, 1, 0, cubeSize)] + x[I3D(N + 1, 0, 1, cubeSize)]) / 3;
    x[I3D(N + 1, 0, N + 1, cubeSize)] = (x[I3D(N, 0, N + 1, cubeSize)] + x[I3D(N + 1, 1, N + 1, cubeSize)] + x[I3D(N + 1, 0, N, cubeSize)]) / 3;
    x[I3D(N + 1, N + 1, 0, cubeSize)] = (x[I3D(N, N + 1, 0, cubeSize)] + x[I3D(N + 1, N, 0, cubeSize)] + x[I3D(N + 1, N + 1, 1, cubeSize)]) / 3;
    x[I3D(N + 1, N + 1, N + 1, cubeSize)] = (x[I3D(N, N + 1, N + 1, cubeSize)] + x[I3D(N + 1, N, N + 1, cubeSize)] + x[I3D(N + 1, N + 1, N, cubeSize)]) / 3;
}

void ExplosionSimulation::project(float *u, float *v, float *w, float *p, float *div) {
    int cubeSize = getArraysSize();

    for (int k = 1; k <= N; ++k) {
        for (int j = 1; j <= N; ++j) {
            for (int i = 1; i <= N; ++i) {
                div[I3D(i, j, k, cubeSize)] =
                        -(u[I3D(i + 1, j, k, cubeSize)] - u[I3D(i - 1, j, k, cubeSize)] + v[I3D(i, j + 1, k, cubeSize)] - v[I3D(i, j - 1, k, cubeSize)] +
                          w[I3D(i, j, k + 1, cubeSize)] -
                          w[I3D(i, j, k - 1, cubeSize)]) / 3.0f / N;
                p[I3D(i, j, k, cubeSize)] = 0.0;
            }
        }
    }
    setBoundaries(NO_DIR, div, N);
    setBoundaries(NO_DIR, p, N);

    for (int step = 0; step < relaxationSteps; ++step) {
        for (int k = 1; k <= N; ++k) {
            for (int j = 1; j <= N; ++j) {
                for (int i = 1; i <= N; ++i) {
                    p[I3D(i, j, k, cubeSize)] =
                            (div[I3D(i, j, k, cubeSize)] + p[I3D(i - 1, j, k, cubeSize)] + p[I3D(i + 1, j, k, cubeSize)] + p[I3D(i, j - 1, k, cubeSize)] +
                             p[I3D(i, j + 1, k, cubeSize)] +
                             p[I3D(i, j, k - 1, cubeSize)] + p[I3D(i, j, k + 1, cubeSize)]) / 6.0f;
                }
            }
        }
        setBoundaries(NO_DIR, p, N);
    }

    for (int k = 1; k <= N; ++k) {
        for (int j = 1; j <= N; ++j) {
            for (int i = 1; i <= N; ++i) {
                int idx3D = I3D(i, j, k, cubeSize);
                u[idx3D] -= (p[I3D(i + 1, j, k, cubeSize)] - p[I3D(i - 1, j, k, cubeSize)]) / 2.0f * N;
                v[idx3D] -= (p[I3D(i, j + 1, k, cubeSize)] - p[I3D(i, j - 1, k, cubeSize)]) / 2.0f * N;
                w[idx3D] -= (p[I3D(i, j, k + 1, cubeSize)] - p[I3D(i, j, k - 1, cubeSize)]) / 2.0f * N;
            }
        }
    }
    setBoundaries(X_DIR, u, N);
    setBoundaries(Y_DIR, v, N);
    setBoundaries(Z_DIR, w, N);
}

void ExplosionSimulation::advect(BoundDirection dir, float *d, float *d0, float *u, float *v, float *w, int N, float dt) {
    int cubeSize = N + 2;

    for (int k = 1; k <= N; ++k) {
        for (int j = 1; j <= N; ++j) {
            for (int i = 1; i <= N; ++i) {
                // i, j, k - współrzędne komórki, dla której liczymy nowe dane
                // x, y, z - współrzędne punktu, z którego przyszły dane idąc wstecz po wektorze prędkości (nie pokrywa się z żadną komórką 1:1)
                int idx3D = I3D(i, j, k, cubeSize);
                float x = i - dt * N * u[idx3D];
                float y = j - dt * N * v[idx3D];
                float z = k - dt * N * w[idx3D];

                // pilnujemy przekraczania granic podczas szukania
                if (x < 0.5f)
                    x = 0.5f;
                if (x > N + 0.5f)
                    x = N + 0.5f;
                if (y < 0.5f)
                    y = 0.5f;
                if (y > N + 0.5f)
                    y = N + 0.5f;
                if (z < 0.5f)
                    z = 0.5f;
                if (z > N + 0.5f)
                    z = N + 0.5f;

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
                d[idx3D] =
                        r0 * s0 * t0 * d0[I3D(i0, j0, k0, cubeSize)] +
                        r0 * s0 * t1 * d0[I3D(i0, j0, k1, cubeSize)] +
                        r0 * s1 * t0 * d0[I3D(i0, j1, k0, cubeSize)] +
                        r0 * s1 * t1 * d0[I3D(i0, j1, k1, cubeSize)] +
                        r1 * s0 * t0 * d0[I3D(i1, j0, k0, cubeSize)] +
                        r1 * s0 * t1 * d0[I3D(i1, j0, k1, cubeSize)] +
                        r1 * s1 * t0 * d0[I3D(i1, j1, k0, cubeSize)] +
                        r1 * s1 * t1 * d0[I3D(i1, j1, k1, cubeSize)];

            }
        }
    }
    setBoundaries(dir, d, N);
}

int ExplosionSimulation::I3D(int i, int j, int k, int cubeSize) {
    return i * cubeSize * cubeSize + j * cubeSize + k;
}
