#include "simulation/ExplosionSimulation.h"
#include "utils/Config.h"
#include "utils/Timer.h"

ExplosionSimulation *simulation;

int main(int argc, char **argv) {
    if (argc != 2) {
        Logger::getInstance().error("Błędne wywołanie, spróbuj tak:");
        Logger::getInstance().error("%s plik_konfiguracyjny", argv[0]);
        exit(0);
    }

    string configFileName = string(argv[1]);
    Config::init(configFileName);

    Logger::getInstance().info("Początek przetwarzania %s", configFileName.c_str());

    simulation = new ExplosionSimulation();

    int currentFrame = 0;
    while (currentFrame <= Config::getInstance()->maxFrames) {
        currentFrame = Timer::getInstance().getCurrentFrame();
        Timer::getInstance().incrementFrame();

        if (currentFrame % Config::getInstance()->framesToSkipRender != 0) {
            continue;
        }

        simulation->proceed();
    }

    Logger::getInstance().info("Koniec przetwarzania %s", configFileName.c_str());

    return 0;

}
