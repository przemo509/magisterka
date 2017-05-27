#include <cstring>
#include "ExternalRenderer.h"

/**
 * Obsługa wizualizacji w programie Blender.
 */
ExternalRenderer::ExternalRenderer(ExplosionSimulation *simulation) {
    dataDirectoryWithPrefix = Config::getInstance()->dataDirectoryWithPrefix;
    this->simulation = simulation;
}

void ExternalRenderer::renderFrame(int frame) {
    renderFrame(frame, "small", Config::getInstance()->saveSmallDensity,
                simulation->getDensityArray(), simulation->getArraysSize());

    if (Config::getInstance()->useWaveletTurbulence()) {
        renderFrame(frame, "big", Config::getInstance()->saveBigDensity,
                    simulation->waveletTurbulence->getDensityBig(), simulation->waveletTurbulence->getResBig().x);
    }
}

void ExternalRenderer::renderFrame(int frame, string densityFilePrefix, int saveFrames, float *densityArray, int arraySize) {
    string densityFilePath = dataDirectoryWithPrefix + "_density_" + densityFilePrefix + "_" + intToString(frame, 3, '0') + ".raw";
    dumpDensity(densityFilePath, densityArray, arraySize);
    runBlender(densityFilePath, densityFilePrefix, frame, arraySize);
    if (shouldRemove(frame, saveFrames)) {
        std::remove(densityFilePath.c_str());
    } else if (Config::getInstance()->zipRawFiles) {
        zipDensityFile(densityFilePath);
        std::remove(densityFilePath.c_str());
    }
}

void ExternalRenderer::dumpDensity(string filePath, float *density, int size) {
    ofstream file(filePath, ios::binary | ios::out);
    if (!file.is_open()) {
        Logger::getInstance()->error("Blad otwarcia pliku [%s]. Szczegoly:\nkod=[%d]\nkomunikat=[%s]\n\nKoncze prace!",
                                     filePath.c_str(), errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    BYTE *out = composeOutArray(density, size);
    file.write((char *) out, size * size * size);

    file.close();
    delete[] out;
    Logger::getInstance()->debug2("Utworzono plik do eksportu: %s", filePath.c_str());
}

BYTE *ExternalRenderer::composeOutArray(float *density, int size) {
    BYTE *out = new BYTE[size * size * size];
    int outIdx = 0;

    for (int j = 0; j < size; ++j) { // zamienione j i k, bo Blender ma w górę oś Z, a ja Y
        for (int k = 0; k < size; ++k) {
            for (int i = 0; i < size; ++i) {
                int densInt = (int) (255.0 * density[I3D(i, j, k, size)]);
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

void ExternalRenderer::runBlender(string densityFilePath, string outputFilePrefix, int frame, int size) {
    Config *config = Config::getInstance();
    string blenderCmd = config->blenderExecutablePath +
                        " --background" +
                        " " + config->blenderScenePath +
                        " --python " + config->pythonScriptPath +
                        " --render-output " + dataDirectoryWithPrefix + "_blender_render_" + outputFilePrefix + "_###.png" +
                        " --render-frame " + intToString(frame) +
                        " --" +
                        " " + densityFilePath +
                        " " + intToString(size) +
                        " " + intToString(size) +
                        " " + intToString(size) +
                        " " + intToString(config->imgResX) +
                        " " + intToString(config->imgResY) +
                        " " + intToString(config->imgResPercentage) +
                        " " + floatToString(config->rayTracingStepSize) +
                        " " + intToString(config->threads) +
                        " " + (frame == 1 ? config->mainConfigFilePath : config->configFilePath) +
                        " \"frame           = " + intToString(frame, 6, '0') + "\"" +
                        " \"verticesActive  = " + intToString(simulation->vertices->getCurrentCount(), 6, '0') + "\"" +
                        " \"verticesTotal   = " + intToString(simulation->vertices->getCreatedCount(), 6, '0') + "\"" +
                        " \"verticesRemoved = " + intToString(simulation->vertices->getDeletedCount(), 6, '0') + "\"" +
                        " > " + dataDirectoryWithPrefix + "_blender_" + outputFilePrefix + "_log.txt 2>&1";

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
    makeVideo(frames, "small", Config::getInstance()->saveSmallFrames);
    if (Config::getInstance()->useWaveletTurbulence()) {
        makeVideo(frames, "big", Config::getInstance()->saveBigFrames);
    }
}

void ExternalRenderer::makeVideo(int frames, string outputFilePrefix, int saveFrames) {
    string outputVideoFilePath = dataDirectoryWithPrefix + " - " + Config::getInstance()->configDescription + "_" + outputFilePrefix + ".mp4";
    string cmd = Config::getInstance()->ffmpegExecutablePath +
                 " -y" +
                 " -loglevel panic" +
                 " -i " + dataDirectoryWithPrefix + "_blender_render_" + outputFilePrefix + "_%03d.png" +
                 " -c:v libx264" +
                 " -crf " + intToString(Config::getInstance()->ffmpegCRF) +
                 " -pix_fmt yuv420p" +
                 " \"" + outputVideoFilePath + "\"";

    int code = system(cmd.c_str());
    if (code != 0) {
        std::cout << cmd << endl;
        Logger::getInstance()->error("Błąd komendy:\n%s", cmd.c_str());
        exit(code);
    } else {
        Logger::getInstance()->info("Film %s zmontowany", outputVideoFilePath.c_str());
    }

    removeRenderedFrames(frames, outputFilePrefix, saveFrames);
}

void ExternalRenderer::removeRenderedFrames(int frames, string outputFilePrefix, int saveFrames) {
    for (int frame = 1; frame <= frames; frame++) {
        if (shouldRemove(frame, saveFrames)) {
            string frameFilePath = dataDirectoryWithPrefix + "_blender_render_" + outputFilePrefix + "_" + intToString(frame, 3, '0') + ".png";
            std::remove(frameFilePath.c_str());
        }
    }
}

int ExternalRenderer::I3D(int i, int j, int k, int cubeSize) {
    return ExplosionSimulation::I3D(i, j, k, cubeSize);
}

bool ExternalRenderer::shouldRemove(int frame, int saveFrames) {
    if (saveFrames == 0) {
        return true; // usuwamy wszystko
    } else if (saveFrames == 1) {
        return false; // zachowujemy wszystko
    } else {
        return frame % saveFrames != 0;
    }
}

void ExternalRenderer::zipDensityFile(string filePath) {
    string cmd = "zip -j " + dataDirectoryWithPrefix + "_all_raws.zip " + filePath;

    int code = system(cmd.c_str());
    if (code != 0) {
        std::cout << cmd << endl;
        Logger::getInstance()->error("Błąd komendy:\n%s", cmd.c_str());
        exit(code);
    } else {
        Logger::getInstance()->debug2("Plik %s dołączony do archiwum", filePath.c_str());
    }
}
