#ifndef TIME_H
#define TIME_H

// Timer class
class Timer
{
private:
    float startTime; // The time when the timer started running

public:
    Timer();
    void reset();
    void update();
    float deltaTime();

};

#endif