#ifndef MAGISTERKA_BLENDEREXPORT_H
#define MAGISTERKA_BLENDEREXPORT_H

#include <cstring>
#include "GeometryUtils.h"
#include "Config.h"

typedef unsigned char BYTE;

static void exportDensity(vect3f density, bool createNewFile) {
    string exportFilePath = Config::getInstance()->exportDirectory + "/" + Config::getInstance()->configName + "_8bit.raw";
    int size = Config::getInstance()->simulationSpaceSize + 2;
    BYTE *out = new BYTE[size * size * size];
    int outIdx = 0;

    for (int j = 0; j < size; ++j) {
        for (int k = 0; k < size; ++k) {
            for (int i = 0; i < size; ++i) {
                int densInt = (int) (255.0 * density[i][j][k]);
                BYTE densByte = (BYTE) densInt;
                if (densInt < 0) {
                    Logger::getInstance().debug4("Komorka [%d, %d, %d] mniejsza od 0: %d", i, j, k, densInt);
                    densByte = 0;
                } else if (densInt > 255) {
                    Logger::getInstance().debug4("Komorka [%d, %d, %d] wieksza od 255: %d", i, j, k, densInt);
                    densByte = 255;
                }
                out[outIdx++] = densByte;
            }
        }
    }

    ofstream file(exportFilePath.c_str(), ios::binary | (createNewFile ? ios::out : ios::app));
    if (!file.is_open()) {
        Logger::getInstance().error("Blad otwarcia pliku [%s]. Szczegoly:\nkod=[%d]\nkomunikat=[%s]\n\nKoncze prace!",
                                    exportFilePath.c_str(), errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    file.write((char *) out, outIdx);
    file.close();

    if (createNewFile) {
        Logger::getInstance().info("Utworzono plik do eksportu: %s", exportFilePath.c_str());
    } else {
        Logger::getInstance().debug1("Klatka %d. Wyeksportowano %d wartosci.", Timer::getInstance().getCurrentFrame(), outIdx);

    }
}

static void exportDensity(vect3f density) {
    exportDensity(density, false);
}

#endif //MAGISTERKA_BLENDEREXPORT_H
