#include <regex>
#include <omp.h>
#include "simulation/ExplosionSimulation.h"
#include "utils/Timer.h"
#include "utils/ExternalRenderer.h"

void logFrameProcessingTime(int currentFrame, long frameStartTime, long programStartTime) {
    // obliczenie szacowanego czasu zakończenia programu
    float percent = 100.0f * currentFrame / Config::getInstance()->maxFrames;
    time_t currentTime = Timer::getInstance().getCurrentTime();
    time_t runningTime = currentTime - programStartTime;
    long estimatedTime = (int) (runningTime * 100.0f / percent - runningTime);
    struct tm *now = localtime(&currentTime);

    Logger::getInstance()->info("Klatka %s z %d (%s%%) wygenerowana w %s s. Koniec za: %s, czyli o: %s",
                                intToString(currentFrame, 3, ' ').c_str(),
                                Config::getInstance()->maxFrames,
                                intToString((int) percent, 3, ' ').c_str(),
                                intToString((int) (currentTime - frameStartTime), 3, ' ').c_str(),
                                formatTime(estimatedTime).c_str(),
                                formatTime((now->tm_hour * 60 + now->tm_min) * 60 + now->tm_sec + estimatedTime).c_str()
    );
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Błędne wywołanie, spróbuj tak:";
        cerr << "%s plik_konfiguracyjny" << argv[0] << endl;
        exit(EXIT_FAILURE);
    }

    long programStartTime = Timer::getInstance().getCurrentTime();

    string configName = string(argv[1]);
    Config::init(configName);
    omp_set_num_threads(Config::getInstance()->threads);

    Logger::getInstance()->info("Początek przetwarzania %s", configName.c_str());

    // Klatka nr 1, pusta symulacja.
    int currentFrame = Timer::getInstance().incrementFrame();
    long frameStartTime = Timer::getInstance().getCurrentTime();
    ExplosionSimulation *simulation = new ExplosionSimulation();
    ExternalRenderer *renderer = new ExternalRenderer(simulation);
    renderer->renderFrame(currentFrame);
    logFrameProcessingTime(currentFrame, frameStartTime, programStartTime);

    while (currentFrame < Config::getInstance()->maxFrames) {
        currentFrame = Timer::getInstance().incrementFrame();
        frameStartTime = Timer::getInstance().getCurrentTime();

        if (currentFrame % Config::getInstance()->framesToSkipRender != 0) {
            continue;
        }

        simulation->proceed();
        renderer->renderFrame(currentFrame);
        logFrameProcessingTime(currentFrame, frameStartTime, programStartTime);
    }

    renderer->makeVideo(currentFrame);

    long totalElapsedTime = Timer::getInstance().getCurrentTime() - programStartTime;
    
    string commandOnFinish = Config::getInstance()->commandOnFinish;
    string shortMsg = "Przetwarzanie " + configName + " zakończyło się po czasie " + formatTime(totalElapsedTime);
    string fullMsg = "Przetwarzanie " + configName + " zakończyło się po czasie " + formatTime(totalElapsedTime);
    commandOnFinish = regex_replace(commandOnFinish, regex("\\$shortMsg"), shortMsg);
    commandOnFinish = regex_replace(commandOnFinish, regex("\\$fullMsg"), fullMsg);
    system(commandOnFinish.c_str());
    
    Logger::getInstance()->info("Koniec przetwarzania %s", configName.c_str());
    Logger::getInstance()->info("Całkowity czas trwania: %s", formatTime(totalElapsedTime).c_str());
    Logger::getInstance()->info("Czas trwania na klatkę (średnio): %s", formatTime(totalElapsedTime / currentFrame).c_str());

    delete renderer;
    delete simulation;
    return 0;

}
