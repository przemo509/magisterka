#include "simulation/ExplosionSimulation.h"
#include "utils/Timer.h"
#include "utils/ExternalRenderer.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Błędne wywołanie, spróbuj tak:";
        cerr << "%s plik_konfiguracyjny" << argv[0];
        exit(EXIT_FAILURE);
    }

    string configName = string(argv[1]);
    Config::init(configName);

    Logger::getInstance()->info("Początek przetwarzania %s", configName.c_str());

    // Klatka nr 1, pusta symulacja.
    int currentFrame = Timer::getInstance().incrementFrame();
    ExplosionSimulation *simulation = new ExplosionSimulation();
    ExternalRenderer *renderer = new ExternalRenderer(simulation);
    renderer->renderFrame(currentFrame);

    while (currentFrame < Config::getInstance()->maxFrames) {
        currentFrame = Timer::getInstance().incrementFrame();

        if (currentFrame % Config::getInstance()->framesToSkipRender != 0) {
            continue;
        }

        simulation->proceed();
        renderer->renderFrame(currentFrame);
    }

    renderer->makeVideo(currentFrame);

    Logger::getInstance()->info("Koniec przetwarzania %s", configName.c_str());

    delete renderer;
    delete simulation;
    return 0;

}
