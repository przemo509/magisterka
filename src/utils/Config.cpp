#include "Config.h"

const string Config::BOOL_TRUE_STRING = "true";

Config *Config::instance;

Config::Config(string configName) {
    this->configName = configName;
    readConfig(true);
    readConfig(false);
}

/**
 * Odczytanie zawartości głównego lub pomocniczego pliku konfiguracyjnego.
 */
void Config::readConfig(bool isMain) {
    string filePath = isMain ? mainConfigFilePath : configFilePath;
    ifstream file(filePath.c_str(), ios::in);
    if (!file.is_open()) {
        cerr << "Nie mogę otworzyć pliku do odczytu: " << filePath << endl;
        exit(EXIT_FAILURE);
    }

    string line;
    int readValues = 0;
    if (!isMain) {
        getline(file, line);
        line[0] = ' '; // pozbycie się #
        line = trim(line); // pozbycie się spacji
        configDescription = line;
    }
    while (getline(file, line)) {
        if (line.length() == 0 || line[0] == '\n' || line[0] == '\r' || line[0] == '#') {
            continue;
        }
        vector<string> lineItems = split(line, '=');
        string key = lineItems[0];
        key = trim(key);
        string value = lineItems.size() > 1 ? lineItems[1] : "";
        value = trim(value);

        if (key == "dataDirectory") {
            dataDirectoryWithPrefix = value + "/" + configName + "/" + configName;
            configFilePath = dataDirectoryWithPrefix + ".txt";
            Logger::init(dataDirectoryWithPrefix);
        } else if (key == "blenderExecutablePath") blenderExecutablePath = value;
        else if (key == "pythonScriptPath") pythonScriptPath = value;
        else if (key == "blenderScenePath") blenderScenePath = value;
        else if (key == "ffmpegExecutablePath") ffmpegExecutablePath = value;
        else if (key == "commandOnFinish") commandOnFinish = value;
        else if (key == "maxFrames") maxFrames = atoi(value.c_str());
        else if (key == "verticesCountFactor") verticesCountFactor = atof(value.c_str());
        else if (key == "vortexStrength") vortexStrength = atof(value.c_str());
        else if (key == "vortexMoving") vortexMoving = atof(value.c_str());
        else if (key == "vortexRadiusMin") vortexRadiusMin = atof(value.c_str());
        else if (key == "vortexRadiusMax") vortexRadiusMax = atof(value.c_str());
        else if (key == "waveletTurbulenceAmplify") waveletTurbulenceAmplify = atoi(value.c_str());
        else if (key == "thermalBuoyancyFactor") thermalBuoyancyFactor = atof(value.c_str());
        else if (key == "gravityFactor") gravityFactor = atof(value.c_str());
        else if (key == "windFactor") windFactor = atof(value.c_str());
        else if (key == "viscosity") viscosity = atof(value.c_str());
        else if (key == "diffusionRate") diffusionRate = atof(value.c_str());
        else if (key == "solidVolumeBoundaries") solidVolumeBoundaries = BOOL_TRUE_STRING == value;
        else if (key == "sourcePosition") sourcePositionString = value.c_str();
        else if (key == "sourceRadius") sourceRadius = atoi(value.c_str());
        else if (key == "sourceHeight") sourceHeight = atoi(value.c_str());
        else if (key == "sourceSpreadFactor") sourceSpreadFactor = atof(value.c_str());
        else if (key == "sourceDensityFactor") sourceDensityFactor = atof(value.c_str());
        else if (key == "sourceVelocitiesValues") sourceVelocitiesValuesString = value;
        else if (key == "sourcePhaseFrames") sourcePhaseFramesString = value;
        else if (key == "sourcePhaseVelocitiesIdxs") sourcePhaseVelocitiesIdxsString = value;
        else if (key == "renderStartsAtFrame") renderStartsAtFrame = atoi(value.c_str());
        else if (key == "renderFrames") renderFrames = atoi(value.c_str());
        else if (key == "imgResX") imgResX = atoi(value.c_str());
        else if (key == "imgResY") imgResY = atoi(value.c_str());
        else if (key == "imgResPercentage") imgResPercentage = atoi(value.c_str());
        else if (key == "rayTracingStepSize") rayTracingStepSize = atof(value.c_str());
        else if (key == "ffmpegCRF") ffmpegCRF = atoi(value.c_str());
        else if (key == "saveSmallDensity") saveSmallDensity = atoi(value.c_str());
        else if (key == "saveBigDensity") saveBigDensity = atoi(value.c_str());
        else if (key == "saveSmallFrames") saveSmallFrames = atoi(value.c_str());
        else if (key == "saveBigFrames") saveBigFrames = atoi(value.c_str());
        else if (key == "zipRawFiles") zipRawFiles = BOOL_TRUE_STRING == value;
        else if (key == "threads") threads = atoi(value.c_str());
        else {
            Logger::getInstance()->error("Nieznany parametr konfiguracyjny: [%s = %s]", key.c_str(), value.c_str());
            exit(EXIT_FAILURE);
        }

        Logger::getInstance()->debug1(line);
        readValues++;
    }

    if (isMain && readValues != requiredMainConfigValues) {
        Logger::getInstance()->error("Brak parametów konfiguracyjnych w liczbie: %d", requiredMainConfigValues - readValues);
        exit(EXIT_FAILURE);
    }

    file.close();
    Logger::getInstance()->info("Poprawnie odczytano plik konfiguracyjny: [%s]", filePath.c_str());
}

/**
 * Funkcja pomocnicza służąca do sprawdzenia czy dana funkcja jest dostępna w danej klatce.
 * Na przykład można dzięki niej usunąć wynikowe pliki pośrednie lub przeprowadzić wizualizację tylko dla wybranych klatek zaczynając od podanej klatki.
 */
bool Config::featureEnabledAtFrame(int frame, int enabledFrames, int startingFrame) {
    if (enabledFrames == 0) {
        return false; // funkcja wyłączona
    } else if (frame < startingFrame) {
        return false; // funkcja wyłączona przed klatką rozpoczynającą
    } else if (enabledFrames == 1) {
        return true; // funkcja włączona dla każdej klatki po klatce rozpoczynającej
    } else {
        return frame % enabledFrames == 0; // funkcja włączona dla co n-tej klatki
    }
}