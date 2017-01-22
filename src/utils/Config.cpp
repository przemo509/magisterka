#include "Config.h"

Config *Config::instance;

Config::Config(string configName) {
    this->configName = configName;

    readMainConfig(".\\config.txt");
    readConfig();

    configureExplosionSource();
}

void Config::readMainConfig(string filePath) {
    ifstream file(filePath.c_str(), ios::in);
    if (!file.is_open()) {
        cerr << "Nie mogę otworzyć pliku do odczytu: " << filePath;
        exit(EXIT_FAILURE);
    }

    string line;
    int readValues = 0;
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
        }
        else if (key == "blenderExecutablePath") blenderExecutablePath = value;
        else if (key == "pythonScriptPath") pythonScriptPath = value;
        else if (key == "blenderScenePath") blenderScenePath = value;
        else if (key == "ffmpegExecutablePath") ffmpegExecutablePath = value;
        else {
            Logger::getInstance()->error("Nieznany parametr konfiguracyjny: [%s = %s]", key.c_str(), value.c_str());
            exit(EXIT_FAILURE);
        }

        Logger::getInstance()->debug1(line);
        readValues++;
    }

    if (readValues != requiredMainConfigValues) {
        Logger::getInstance()->error("Brak parametów konfiguracyjnych w liczbie: %d", requiredMainConfigValues - readValues);
        exit(EXIT_FAILURE);
    }

    file.close();
    Logger::getInstance()->info("Poprawnie odczytano główny plik konfiguracyjny: [%s]", filePath.c_str());
}

void Config::readConfig() {
    Logger::getInstance()->info("Odczytuję plik konfiguracyjny: [%s]", configFilePath.c_str());
    ifstream file(configFilePath.c_str(), ios::in);
    if (!file.is_open()) {
        Logger::getInstance()->error("Nie mogę otworzyć pliku do odczytu: [%s]", configFilePath.c_str());
        exit(EXIT_FAILURE);
    }

    string line;
    int readValues = 0;
    while (getline(file, line)) {
        Logger::getInstance()->debug1(line);

        if (line[0] == '#') {
            continue;
        }
        vector<string> lineItems = split(line, '=');
        string key = trim(lineItems[0]);
        string value = trim(lineItems[1]);

        if (key == "verticesCount") verticesCount = atoi(value.c_str());
        else if (key == "vortexStrength") vortexStrength = atof(value.c_str());
        else if (key == "vortexMoving") vortexMoving = atoi(value.c_str());
        else if (key == "vortexRadiusMin") vortexRadiusMin = atof(value.c_str());
        else if (key == "vortexRadiusMax") vortexRadiusMax = atof(value.c_str());
        else if (key == "maxFrames") maxFrames = atoi(value.c_str());
        else if (key == "thermalBuoyancyFactor") thermalBuoyancyFactor = atof(value.c_str());
        else if (key == "gravityFactor") gravityFactor = atof(value.c_str());
        else if (key == "windFactor") windFactor = atof(value.c_str());
        else if (key == "viscosity") viscosity = atof(value.c_str());
        else if (key == "diffusionRate") diffusionRate = atof(value.c_str());
        else if (key == "mainSourceDensity") mainSourceDensity = atof(value.c_str());
        else if (key == "vySlow") vySlow = atof(value.c_str());
        else if (key == "vyMedium") vyMedium = atof(value.c_str());
        else if (key == "vyFast") vyFast = atof(value.c_str());
        else {
            Logger::getInstance()->error("Nieznany parametr konfiguracyjny: [%s = %s]", key.c_str(), value.c_str());
            exit(EXIT_FAILURE);
        }

        readValues++;
    }

    if (readValues != requiredConfigValues) {
        Logger::getInstance()->error("Brak parametów konfiguracyjnych w liczbie: %d", requiredConfigValues - readValues);
        exit(EXIT_FAILURE);
    }

    file.close();
    Logger::getInstance()->info("Poprawnie odczytano plik konfiguracyjny: [%s]", configFilePath.c_str());
}

void Config::configureExplosionSource() {
    mainSourceVY = vySlow;
    int step = 0;
    explosionSource[step][0] = 0;
    explosionSource[step++][1] = 0.0;
    explosionSource[step][0] = 12;
    explosionSource[step++][1] = vySlow;
    explosionSource[step][0] = 15;
    explosionSource[step++][1] = vyMedium;
    explosionSource[step][0] = 18;
    explosionSource[step++][1] = vyFast;
    explosionSource[step][0] = 80;
    explosionSource[step++][1] = vySlow;
    explosionSource[step][0] = 140;
    explosionSource[step++][1] = vyFast;
    explosionSource[step][0] = 180;
    explosionSource[step++][1] = vySlow;
    explosionSource[step][0] = 220;
    explosionSource[step++][1] = vyFast;
    explosionSource[step][0] = 280;
    explosionSource[step++][1] = vySlow;
    explosionSource[step][0] = maxFrames;
    explosionSource[step++][1] = 0.0;
    explosionSourcePhases = sizeof(explosionSource) / sizeof(explosionSource[0]);
}
