#ifndef MAGISTERKA_TIMER_H
#define MAGISTERKA_TIMER_H

class Timer {
public:

    static Timer &getInstance() {
        static Timer instance;
        return instance;
    }

    int incrementFrame();

    long getCurrentTime();

    int getCurrentFrame();

private:

    Timer();

    Timer(Timer const &);

    void operator=(Timer const &);

    virtual ~Timer();

    int currentFrame;
    int framesInSecond;
    long prevTime;
};

#endif //MAGISTERKA_TIMER_H
