#include "simulation/ExplosionSimulation.h"
#include "utils/Timer.h"
#include "utils/ExternalRenderer.h"

void logFrameProcessingTime(int currentFrame, long frameStartTime, long programStartTime) {
    // obliczenie szacowanego czasu zakończenia programu
    float percent = 100.0f * currentFrame / Config::getInstance()->maxFrames;
    long currentTime = Timer::getInstance().getCurrentTime();
    long runningTime = currentTime - programStartTime;
    long etaS = (int) (runningTime * 100.0f / percent - runningTime);
    long etaH = etaS / 3600;
    etaS -= etaH * 3600;
    long etaM = etaS / 60;
    etaS -= etaM * 60;
    struct tm *estimatedEnd = localtime(&currentTime);
    estimatedEnd->tm_sec += etaS;
    estimatedEnd->tm_min += etaM + estimatedEnd->tm_sec / 60;
    estimatedEnd->tm_hour += etaH + estimatedEnd->tm_min / 60;
    estimatedEnd->tm_sec %= 60;
    estimatedEnd->tm_min %= 60;
    estimatedEnd->tm_hour %= 24;

    Logger::getInstance()->info("Klatka %s z %d (%s%%) wygenerowana w %s s. Koniec za: %s:%s:%s, czyli o: %s:%s:%s",
                                intToString(currentFrame, 3, ' ').c_str(),
                                Config::getInstance()->maxFrames,
                                intToString((int) percent, 3, ' ').c_str(),
                                intToString(currentTime - frameStartTime, 3, ' ').c_str(),
                                intToString(etaH, 2, '0').c_str(),
                                intToString(etaM, 2, '0').c_str(),
                                intToString(etaS, 2, '0').c_str(),
                                intToString(estimatedEnd->tm_hour, 2, '0').c_str(),
                                intToString(estimatedEnd->tm_min, 2, '0').c_str(),
                                intToString(estimatedEnd->tm_sec, 2, '0').c_str()
    );
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Błędne wywołanie, spróbuj tak:";
        cerr << "%s plik_konfiguracyjny" << argv[0];
        exit(EXIT_FAILURE);
    }

    long programStartTime = Timer::getInstance().getCurrentTime();

    string configName = string(argv[1]);
    Config::init(configName);

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

    Logger::getInstance()->info("Koniec przetwarzania %s", configName.c_str());
    Logger::getInstance()->info("Czas trwania: %d s", Timer::getInstance().getCurrentTime() - programStartTime);

    delete renderer;
    delete simulation;
    return 0;

}
