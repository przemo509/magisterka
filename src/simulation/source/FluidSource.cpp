#include "FluidSource.h"
#include "../../utils/Timer.h"

FluidSource::FluidSource(Config *config) {
    radius = 6;
    height = 3;
    positionX = 0 + radius + 5; // po lewej stronie, trochę dalej od krawędzi
    positionY = 2; // tuż nad brzegiem
    positionZ = Config::getInstance()->simulationSpaceSize / 2; // po środku
    density = config->sourceDensity;
    spreadFactor = 0.35;

    vector<float> velocitiesValues = parseVelocities(config->sourceVelocitiesValuesString);
    vector<int> phaseFrames = parsePhaseFrames(config->sourcePhaseFramesString);
    vector<int> phaseVelocitiesIdxs = parsePhaseVelocities(config->sourcePhaseVelocitiesIdxsString);
    calculateVelocitiesDistribution(velocitiesValues, phaseFrames, phaseVelocitiesIdxs);
}

FluidSource::~FluidSource() {
    delete[] velocitiesDistribution;
}

vector<float> FluidSource::parseVelocities(string velocitiesValuesString) {
    vector<string> velocitiesValuesStrings = split(velocitiesValuesString, Config::numbersDelimiter);
    vector<float> velocitiesValues;
    for (int i = 0; i < velocitiesValuesStrings.size(); ++i) {
        velocitiesValues.push_back((float) atof(trim(velocitiesValuesStrings[i]).c_str()));
    }
    return velocitiesValues;
}

vector<int> FluidSource::parsePhaseFrames(string phaseFramesString) {
    vector<string> phaseFramesStrings = split(phaseFramesString, Config::numbersDelimiter);
    vector<int> phaseFrames;
    for (int i = 0; i < phaseFramesStrings.size(); ++i) {
        phaseFrames.push_back(atoi(trim(phaseFramesStrings[i]).c_str()));
    }
    return phaseFrames;
}

vector<int> FluidSource::parsePhaseVelocities(string phaseVelocitiesIdxsString) {
    vector<string> phaseVelocitiesIdxsStrings = split(phaseVelocitiesIdxsString, Config::numbersDelimiter);
    vector<int> phaseVelocitiesIdxs;
    for (int i = 0; i < phaseVelocitiesIdxsStrings.size(); ++i) {
        phaseVelocitiesIdxs.push_back(atoi(trim(phaseVelocitiesIdxsStrings[i]).c_str()));
    }
    return phaseVelocitiesIdxs;
}

void FluidSource::calculateVelocitiesDistribution(vector<float> velocitiesValues, vector<int> phaseFrames, vector<int> phaseVelocitiesIdxs) {
    if (phaseFrames.size() != phaseVelocitiesIdxs.size()) {
        Logger::getInstance()->error("Liczba faz źródła w klatkach [%d] nie zgadza się z liczbą faz w prędkościach [%d]",
                                     phaseFrames.size(),
                                     phaseVelocitiesIdxs.size());
        exit(0);
    }
    maxFrameForVertices = max(Config::getInstance()->maxFrames + 1, phaseFrames.back()); // + 1, bo klatki numerujemy od 1
    velocitiesDistribution = new float[maxFrameForVertices];
    for (int frame = 0; frame < maxFrameForVertices; ++frame) {
        velocitiesDistribution[frame] = 0.0;
    }

    for (int phase = 0; phase < phaseFrames.size(); ++phase) {
        int phaseStartFrame = phaseFrames[phase];
        int phaseEndFrame = (phase == phaseFrames.size() - 1) // ostatnia faza trwa do końca
                            ? (maxFrameForVertices - 1)
                            : (phaseFrames[phase + 1] - 1);
        for (int frame = phaseStartFrame; frame <= phaseEndFrame; ++frame) {
            velocitiesDistribution[frame] = velocitiesValues[phaseVelocitiesIdxs[phase]];
        }
    }
}

float FluidSource::getCurrentVelocity() {
    return velocitiesDistribution[Timer::getInstance().getCurrentFrame()];
}

int FluidSource::getStartX() {
    return positionX - radius;
}

int FluidSource::getEndX() {
    return positionX + radius;
}

int FluidSource::getStartY() {
    return positionY;
}

int FluidSource::getEndY() {
    return positionY + height;
}

int FluidSource::getStartZ() {
    return positionZ - radius;
}

int FluidSource::getEndZ() {
    return positionZ + radius;
}
