#include <cstring>
#include "ExternalRenderer.h"

/**
 * Obsługa wizualizacji w programie Blender.
 */
ExternalRenderer::ExternalRenderer(ExplosionSimulation *simulation) {
    this->config = Config::getInstance();
    this->simulation = simulation;
}

void ExternalRenderer::renderFrame(int frame) {
    renderFrame(frame, "small", config->saveSmallDensity, simulation->getDensityArray(), simulation->getArraysSize());

    if (config->useWaveletTurbulence()) {
        WTURBULENCE *wt = simulation->waveletTurbulence;
        renderFrame(frame, "big", config->saveBigDensity, wt->getDensityBig(), wt->getResBig().x);
    }
}

void ExternalRenderer::renderFrame(int frame, string densityFilePrefix, int saveFrames, float *densityArray, int arraySize) {
    string densityFilePath = config->dataDirectoryWithPrefix + "_density_" + densityFilePrefix + "_" + intToString(frame, 4, '0') + ".raw";
    dumpDensity(densityFilePath, densityArray, arraySize);
    if (Config::featureEnabledAtFrame(frame, config->renderFrames, config->renderStartsAtFrame)) {
        runBlender(densityFilePath, densityFilePrefix, frame, arraySize);
    }
    if (shouldRemove(frame, saveFrames)) {
        std::remove(densityFilePath.c_str());
    } else if (config->zipRawFiles) {
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
    string blenderCmd = config->blenderExecutablePath +
                        " --background" +
                        " " + config->blenderScenePath +
                        " --python " + config->pythonScriptPath +
                        " --render-output " + config->dataDirectoryWithPrefix + "_blender_render_" + outputFilePrefix + "_####.png" +
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
                        " > " + config->dataDirectoryWithPrefix + "_blender_" + outputFilePrefix + "_log.txt 2>&1";

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
    makeVideo(frames, "small", config->saveSmallFrames);
    if (config->useWaveletTurbulence()) {
        makeVideo(frames, "big", config->saveBigFrames);
    }
}

void ExternalRenderer::makeVideo(int frames, string outputFilePrefix, int saveFrames) {
    string outputVideoFilePath = config->dataDirectoryWithPrefix + " - " + config->configDescription + "_" + outputFilePrefix + ".mp4";
    string framesSequencePrefix = config->dataDirectoryWithPrefix + "_blender_render_" + outputFilePrefix;
    string imagesSequencePrefix = framesSequencePrefix;
    if (needToMakeContinuousSequence()) {
        imagesSequencePrefix = copyFramesContinuously(frames, framesSequencePrefix);
    }
    string cmd = config->ffmpegExecutablePath +
                 " -y" +
                 " -loglevel panic" +
                 " -i " + imagesSequencePrefix + "_%04d.png" +
                 " -c:v libx264" +
                 " -crf " + intToString(config->ffmpegCRF) +
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

    removeRenderedFrames(frames, framesSequencePrefix, saveFrames);
    if (needToMakeContinuousSequence()) {
        removeContinuousCopies(frames, imagesSequencePrefix);
    }
}

bool ExternalRenderer::needToMakeContinuousSequence() const {
    return Config::getInstance()->renderFrames > 1;
}

void ExternalRenderer::removeRenderedFrames(int frames, string imageSequencePrefix, int saveFrames) {
    for (int frame = 1; frame <= frames; frame++) {
        if (shouldRemove(frame, saveFrames)) {
            string frameFilePath = imageSequencePrefix + "_" + intToString(frame, 4, '0') + ".png";
            std::remove(frameFilePath.c_str());
        }
    }
}

int ExternalRenderer::I3D(int i, int j, int k, int cubeSize) {
    return ExplosionSimulation::I3D(i, j, k, cubeSize);
}

bool ExternalRenderer::shouldRemove(int frame, int saveFrames) {
    return !Config::featureEnabledAtFrame(frame, saveFrames, 1);
}

void ExternalRenderer::zipDensityFile(string filePath) {
    string cmd = "zip -j " + config->dataDirectoryWithPrefix + "_all_raws.zip " + filePath;

    int code = system(cmd.c_str());
    if (code != 0) {
        std::cout << cmd << endl;
        Logger::getInstance()->error("Błąd komendy:\n%s", cmd.c_str());
        exit(code);
    } else {
        Logger::getInstance()->debug2("Plik %s dołączony do archiwum", filePath.c_str());
    }
}

string ExternalRenderer::copyFramesContinuously(int frames, string imageSequencePrefix) {
    string imageSequencePrefixNew = imageSequencePrefix + "_continuous";

    int image = 1;
    for (int frame = 1; frame <= frames; frame++) {
        string fileToCopyPath = imageSequencePrefix + "_" + intToString(frame, 4, '0') + ".png";
        string newFilePath = imageSequencePrefixNew + "_" + intToString(image, 4, '0') + ".png";
        std::ifstream fileToCopy(fileToCopyPath, std::ios::binary);
        if (fileToCopy.good()) { // plik istnieje
            std::ofstream newFile(newFilePath, std::ios::binary);
            newFile << fileToCopy.rdbuf();

            fileToCopy.close();
            newFile.close();
            image++;
        }
    }

    return imageSequencePrefixNew;
}

void ExternalRenderer::removeContinuousCopies(int images, string imagesSequencePrefix) {
    for (int image = 1; image <= images; image++) {
        string frameFilePath = imagesSequencePrefix + "_" + intToString(image, 4, '0') + ".png";
        std::remove(frameFilePath.c_str());
    }
}
