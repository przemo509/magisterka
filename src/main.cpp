#include <ctime>
#include <iomanip>
#include "simulation/ExplosionSimulation.h"
#include "utils/Config.h"
#include "utils/Timer.h"

string configFileName;

ExplosionSimulation *simulation;

void printWithTimestamp(string s) {
    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    cout << setfill('0')
         << "["
         << setw(4) << (now->tm_year + 1900) << '-'
         << setw(2) << (now->tm_mon + 1) << '-'
         << setw(2) << now->tm_mday << ' '
         << setw(2) << now->tm_hour << ':'
         << setw(2) << now->tm_min << ':'
         << setw(2) << now->tm_sec << "] "
         << setfill(' ') << s << endl;
}

int main(int argc, char **argv) {
//    setvbuf(stdout, NULL, _IONBF, 0); TODO potrzebne?
//    setvbuf(stderr, NULL, _IONBF, 0); TODO potrzebne?

    if (argc != 2) {
        cerr << "Błędne wywołanie, spróbuj tak:" << endl;
        cerr << argv[0] << " plik_konfiguracyjny" << endl;
        exit(0);
    }

    configFileName = string(argv[1]);
    Config::init(configFileName);

    printWithTimestamp("Początek przetwarzania " + configFileName);

    simulation = new ExplosionSimulation();

    int currentFrame = Timer::getInstance().getCurrentFrame();
    while (currentFrame <= Config::getInstance()->maxFrames) {
        currentFrame = Timer::getInstance().getCurrentFrame();
        Timer::getInstance().incrementFrame();

        if (currentFrame % Config::getInstance()->framesToSkipRender != 0) {
            continue;
        }

        simulation->proceed();

        Logger::getInstance().printOnScreen();

    }

    printWithTimestamp("  Koniec przetwarzania " + configFileName);

    return 0;

}

