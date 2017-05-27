/**
 * Autor:                       Przemysław Sadło
 * Tytuł pracy magisterskiej:   Modelowanie przepływu turbulentnego w grafice komputerowej
 * Data:                        maj 2017r.
 * Instytut Informatyki, Wydział Elektroniki i Technik Informacyjnych, Politechnika Warszawska
 */

#include <regex>
#include <omp.h>
#include "simulation/ExplosionSimulation.h"
#include "utils/Timer.h"
#include "utils/ExternalRenderer.h"

/**
 * Funkcja pomocnicza, której zadaniem jest wypisywanie:
 *  - czasu generowania bieżącej klatki animacji,
 *  - procentowego postępu animacji,
 *  - przewidywanego czasu zakończenia animacji.
 */
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

/**
 * Główna funkcja uruchamiająca symulator.
 * Spodziewa się jednego argumentu z nazwą konfiguracji do uruchomienia.
 * Konfiguracja ta powinna być umieszczona pod ścieżką podaną w głównej konfiguracji symulatora.
 * Natomiast główna konfiguracja symulatora powinna znajdować się w pliku "config.txt" w jego katalogu roboczym.
 */
int main(int argc, char **argv) {
    // Sprawdzenie argumentów.
    if (argc != 2) {
        cerr << "Błędne wywołanie, spróbuj tak:" << endl;
        cerr << argv[0] << " plik_konfiguracyjny" << endl;
        exit(EXIT_FAILURE);
    }

    // Rozpoczęcie pomiaru czasu symulacji.
    long programStartTime = Timer::getInstance().getCurrentTime();

    // Inicjalizacja parametrów symulatora z plików konfiguracyjnych.
    string configName = string(argv[1]);
    Config::init(configName);
    omp_set_num_threads(Config::getInstance()->threads);

    Logger::getInstance()->info("Początek przetwarzania %s", configName.c_str());

    // Inicjalizacja symulatora i wizualizacji.
    ExplosionSimulation *simulation = new ExplosionSimulation();
    ExternalRenderer *renderer = new ExternalRenderer(simulation);

    // Klatka nr 1, pusta scena.
    int currentFrame = Timer::getInstance().incrementFrame();
    long frameStartTime = Timer::getInstance().getCurrentTime();
    renderer->renderFrame(currentFrame);
    logFrameProcessingTime(currentFrame, frameStartTime, programStartTime);

    // Pętla główna programu - kolejne klatki animacji.
    while (currentFrame < Config::getInstance()->maxFrames) {
        currentFrame = Timer::getInstance().incrementFrame();
        frameStartTime = Timer::getInstance().getCurrentTime();

        // Krok symulacji + wizualizacja.
        if (!Config::getInstance()->renderExistingDumps) {
            simulation->proceed();
        }
        renderer->renderFrame(currentFrame);
        logFrameProcessingTime(currentFrame, frameStartTime, programStartTime);
    }

    // Połączenie wygenerowanych klatek w jeden film.
    renderer->makeVideo(currentFrame);

    // Wykonanie komendy końcowej podanej w konfiguracji.
    string commandOnFinish = Config::getInstance()->commandOnFinish;
    long totalElapsedTime = Timer::getInstance().getCurrentTime() - programStartTime;
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
