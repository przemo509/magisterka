#include <cstring>
#include "ExternalRenderer.h"
#include "Config.h"

ExternalRenderer::ExternalRenderer(ExplosionSimulation *simulation) {
    dataDirectoryWithPrefix = Config::getInstance()->dataDirectoryWithPrefix;
    this->simulation = simulation;
    size = this->simulation->getArraysSize();
    cellsCount = size * size * size;
}

void ExternalRenderer::renderFrame(int frame) {
    string densityFilePath = dataDirectoryWithPrefix + "_density_" + intToString(frame, 3, '0') + ".raw";

    dumpDensity(densityFilePath);
    runBlender(densityFilePath, frame);
    std::remove(densityFilePath.c_str());
}

void ExternalRenderer::dumpDensity(string filePath) {
    ofstream file(filePath, ios::binary | ios::out);
    if (!file.is_open()) {
        Logger::getInstance()->error("Blad otwarcia pliku [%s]. Szczegoly:\nkod=[%d]\nkomunikat=[%s]\n\nKoncze prace!",
                                     filePath.c_str(), errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    BYTE *out = composeOutArray();
    file.write((char *) out, cellsCount);

    file.close();
    delete[] out;
    Logger::getInstance()->info("Utworzono plik do eksportu: %s", filePath.c_str());
}

BYTE *ExternalRenderer::composeOutArray() {
    vect3f density = simulation->getDensityArray();
    BYTE *out = new BYTE[cellsCount];
    int outIdx = 0;

    for (int j = 0; j < size; ++j) { // zamienione j i k, bo Blender ma w górę oś Z, a ja Y
        for (int k = 0; k < size; ++k) {
            for (int i = 0; i < size; ++i) {
                int densInt = (int) (255.0 * density[i][j][k]);
                BYTE densByte = (BYTE) densInt;
                if (densInt < 0) {
                    Logger::getInstance()->debug4("Komorka [%d, %d, %d] mniejsza od 0: %d", i, j, k, densInt);
                    densByte = 0;
                } else if (densInt > 255) {
                    Logger::getInstance()->debug4("Komorka [%d, %d, %d] wieksza od 255: %d", i, j, k, densInt);
                    densByte = 255;
                }
                out[outIdx++] = densByte;
            }
        }
    }

    return out;
}

void ExternalRenderer::runBlender(string densityFilePath, int frame) {
    string blenderCmd = Config::getInstance()->blenderExecutablePath +
                        " --background" +
                        " " + Config::getInstance()->blenderScenePath +
                        " --python " + Config::getInstance()->pythonScriptPath +
                        " --render-output " + dataDirectoryWithPrefix + "_blender_render_###.png" +
                        " --render-frame " + intToString(frame) +
                        " --" +
                        " " + densityFilePath +
                        " " + intToString(size) +
                        " " + intToString(size) +
                        " " + intToString(size) +
                        " " + Config::getInstance()->configFilePath +
                        " \"frame                   = " + intToString(frame, 3, '0') + "\"" +
                        " > " + dataDirectoryWithPrefix + "_blender_log.txt 2>&1";

    int code = system(blenderCmd.c_str());
    if (code != 0) {
        std::cout << blenderCmd << endl;
        Logger::getInstance()->error("Błąd komendy:\n%s", blenderCmd.c_str());
        exit(code);
    } else {
        Logger::getInstance()->debug2("Klatka %d wyrenderowana", frame);
    }
}

void ExternalRenderer::makeVideo(int frames) {
    string cmd = Config::getInstance()->ffmpegExecutablePath +
                 " -y" +
                 " -loglevel panic" +
                 " -i " + dataDirectoryWithPrefix + "_blender_render_%03d.png" +
                 " -c:v ffv1" +
                 " -qscale:v 0" +
                 " " + dataDirectoryWithPrefix + ".avi";

    int code = system(cmd.c_str());
    if (code != 0) {
        std::cout << cmd << endl;
        Logger::getInstance()->error("Błąd komendy:\n%s", cmd.c_str());
        exit(code);
    } else {
        Logger::getInstance()->info("Film %s.avi zmontowany", dataDirectoryWithPrefix.c_str());
    }

    removeRenderedFrames(frames);
}

void ExternalRenderer::removeRenderedFrames(int frames) {
    for (int frame = 1; frame <= frames; frame++) {
        string frameFilePath = dataDirectoryWithPrefix + "_blender_render_" + intToString(frame, 3, '0') + ".png";
        std::remove(frameFilePath.c_str());
    }
}
