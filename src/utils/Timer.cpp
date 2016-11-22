#include "Timer.h"
#include "Logger.h"

using namespace std;

Timer::Timer() {
    currentFrame = 0;
    framesInSecond = 0;
    currTime = 0;
    prevTime = 0;
}

Timer::~Timer() {
}

void Timer::incrementFrame() {
    ++currentFrame;
    ++framesInSecond;
    currTime = time(NULL);
    Logger::getInstance().debug2("Frame: %i", currentFrame);
    long interval = currTime - prevTime;
    if (interval > 1000) {
        prevTime = currTime;
        Logger::getInstance().debug1("FPS: %f", framesInSecond / (interval / 1000.0f));
        framesInSecond = 0;
    }
}

long Timer::getCurrentTime() {
    return currTime;
}

int Timer::getCurrentFrame() {
    return currentFrame;
}