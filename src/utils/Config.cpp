#include "Config.h"

Config *Config::instance;

Config::Config(string configName) {
    this->configName = configName;
    readConfig(true);
    readConfig(false);
}

void Config::readConfig(bool isMain) {
    string filePath = isMain ? mainConfigFilePath : configFilePath;
    ifstream file(filePath.c_str(), ios::in);
    if (!file.is_open()) {
        cerr << "Nie mogę otworzyć pliku do odczytu: " << filePath << endl;
        exit(EXIT_FAILURE);
    }

    string line;
    int readValues = 0;
    if(!isMain) {
        getline(file, line);
        line[0] = ' '; // pozbycie się #
        line = trim(line); // pozbycie się spacji
        configDescription = line;
    }
    while (getline(file, line)) {
        if (line[0] == '#' || line.length() == 0) {
            continue;
        }
        vector<string> lineItems = split(line, '=');
        string key = trim(lineItems[0]);
        string value = trim(lineItems[1]);

        if (key == "dataDirectory") {
            dataDirectoryWithPrefix = value + "\\" + configName + "\\" + configName;
            configFilePath = dataDirectoryWithPrefix + ".txt";
            Logger::init(dataDirectoryWithPrefix);
        } else if (key == "blenderExecutablePath") blenderExecutablePath = value;
        else if (key == "pythonScriptPath") pythonScriptPath = value;
        else if (key == "blenderScenePath") blenderScenePath = value;
        else if (key == "ffmpegExecutablePath") ffmpegExecutablePath = value;
        else if (key == "maxFrames") maxFrames = atoi(value.c_str());
        else if (key == "verticesCount") verticesCount = atoi(value.c_str());
        else if (key == "vortexStrength") vortexStrength = atof(value.c_str());
        else if (key == "vortexMoving") vortexMoving = atoi(value.c_str());
        else if (key == "vortexRadiusMin") vortexRadiusMin = atof(value.c_str());
        else if (key == "vortexRadiusMax") vortexRadiusMax = atof(value.c_str());
        else if (key == "thermalBuoyancyFactor") thermalBuoyancyFactor = atof(value.c_str());
        else if (key == "gravityFactor") gravityFactor = atof(value.c_str());
        else if (key == "windFactor") windFactor = atof(value.c_str());
        else if (key == "viscosity") viscosity = atof(value.c_str());
        else if (key == "diffusionRate") diffusionRate = atof(value.c_str());
        else if (key == "sourceDensity") sourceDensity = atof(value.c_str());
        else if (key == "svv") sourceVelocitiesValuesString = value;
        else if (key == "spf") sourcePhaseFramesString = value;
        else if (key == "spvi") sourcePhaseVelocitiesIdxsString = value;
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