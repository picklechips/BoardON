#include <SDL2/SDL.h>
#include "time.h"

// Initializing the timers variables
Timer::Timer()
{
    startTime = SDL_GetTicks();
}

// Updating the time
float Timer::deltaTime()
{
    return SDL_GetTicks() - startTime;
}

// Resetting the timer
void Timer::reset()
{
    startTime = SDL_GetTicks();
}