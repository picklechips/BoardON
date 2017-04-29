#ifndef TIME_H
#define TIME_H

// Timer class
class Timer
{
public:
    Timer();
    void reset();
    void update();
    float deltaTime();

private:
    float startTime;

};

#endif