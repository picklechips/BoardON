#include <iostream>
#include <SDL.h>

using namespace std;

int main(int argc, char* args[])
{
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    // Creating the window
    SDL_Window* window = NULL;

    // Creating the screen buffer
    SDL_Surface* screenSurface = NULL;
    // Initializing SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "The SDL could not be initialized! Error: " << SDL_GetError() << endl;
    }
    else
    {
        window = SDL_CreateWindow("SDL WINDOW WADDUP", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            cout << "The window could be be created! SDL_Error: " << SDL_GetError() << endl;
        }
    }


    return 0;
}
