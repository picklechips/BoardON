// All sources are created and copyrighted by Ryan Martin - 2014.
// Do not use or alter any of the files without written consent.

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <iomanip>
#include <ctime> // Used to seed srand();
#include <stdlib.h> // used to generate random numbers
#include <sstream>
#include <fstream> // Used for saving highscores

#include "time.h"
#include "biome.h"
#include "obstacle.h"
#include "game.h"

using namespace std;

/*************************
The main game function
*************************/
int main(int argc, char* args[])
{
    if (!Game::init())
    {
        cout << "Failed to initialize!\n";
    }
    else
    {
        // Load media
        if (!Game::loadMedia())
        {
            cout << "The media failed to load!\n";
        }
        else
        {
            SDL_Event e; // Event handler
            Timer Time; // Creating a timer object
            float crashTimer = 0; // Used to check the time since the last crash
            float playerAirTimer = 0; // Used to check how long the player has been in the air
            float playerJumpCooldown = 0; // Used so the player has to wait a certain time to jump again
            SDL_Color defaultTextColor = {0,0,0};

            // Creating obstacles
            Obstacle cabin("cabin", 0, 0, 128, 128, 20, 70, 88, 48, false, false, 0, Game::Obstacles);
            Obstacle stairs("stairs", 128, 0, 64, 128, 10, 15, 50, 98, false, true, 0, Game::Obstacles);
            Obstacle tree("tree", 192, 0, 64, 64, 28, 50, 3, 2, false, false, 0, Game::Obstacles);
            Obstacle rock("rock", 0, 128, 32, 32, 10, 20, 9, 8, true, false, 25, Game::Obstacles);
            Obstacle bigRock("big rock", 384, 0, 64, 64, 25, 25, 29, 32, false, false, 0, Game::Obstacles);
            Obstacle cliff("cliff", 256, 0, 128, 128, 15, 50, 98, 78, true, true, 60, Game::Obstacles);
            Obstacle jump("jump", 0, 160, 64, 64, true, Game::Obstacles);
            Obstacle log ("log", 192, 128, 64, 32, 7, 5, 50, 15, true, false, 25, Game::Obstacles);

            // Creating biomes
            // Village biome
            vector<Obstacle> biomeObsts = {cabin, stairs, cabin, tree, rock};
            vector<int> amntPerBiome = {0, 3, 12, 20, 8};
            Biome village(biomeObsts, amntPerBiome);

            //Forest biome
            biomeObsts = {cabin, tree, rock, log};
            amntPerBiome = {0, 80, 8, 10};
            Biome forest(biomeObsts, amntPerBiome);

            //Rocky Biome
            biomeObsts = {rock, cliff, bigRock, rock, tree};
            amntPerBiome = {0, 7, 15, 14, 7};
            Biome rockies(biomeObsts, amntPerBiome);

            // Total amount of possible biomes
            const int totBiomes = 7;
            // Store the previous biome and obstacle locations for when new biome loads, still display the old biome thats on the screen
            Biome prevBiome;
            // Storing all the possible biomes
            Biome biomes[] = {forest, forest, forest, village, village, rockies, rockies};
            // The random current biome
            Biome curBiome = Game::RandBiome(biomes, totBiomes);


            // Initializing the trails
            SDL_Texture* trailTexture = Game::loadTexture("../images/trail.bmp"); // Loading the trail texture
            // Creating the background textures to draw the trail to
            SDL_Texture * trail = SDL_CreateTexture(Game::Renderer, SDL_PIXELFORMAT_ABGR1555, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
            // Setting it to be white
            SDL_SetRenderTarget(Game::Renderer, trail);
            SDL_SetRenderDrawColor(Game::Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(Game::Renderer);
            SDL_SetRenderTarget(Game::Renderer, NULL);
            // Creating the background texture to store the previous trails
            SDL_Texture * prevtrail = SDL_CreateTexture(Game::Renderer, SDL_PIXELFORMAT_ABGR1555, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
            // Setting it to be white
            SDL_SetRenderTarget(Game::Renderer, prevtrail);
            SDL_SetRenderDrawColor(Game::Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(Game::Renderer);
            SDL_SetRenderTarget(Game::Renderer, NULL);

            int trailPos = 0;


            // Creating the Camera
            SDL_Rect camera = {0, 0, 640, 480};

            // Bottom of the last loaded biome
            int biomeTop = 0;

            curBiome.load(biomeTop);
            bool quit = false; // Flag to check if user has quit
            bool paused = false; // Flag to check if game is paused
            while (!quit)
            {
                while (SDL_PollEvent( &e ) != 0) // Looping through the event queue
                {
                    if (e.type == SDL_QUIT) // Checking if the current event on the poll is QUIT, if it is set quit to true to end the program
                    {
                        quit = true;
                    }
                    else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_p)
                    {
                        paused = true;
                        Game::RenderText(SCREEN_WIDTH/2-18, SCREEN_HEIGHT/2, "PAUSED", defaultTextColor, Game::font);
                        SDL_RenderPresent(Game::Renderer);
                    }
                }

                while (paused)
                {
                    while (SDL_PollEvent( &e ) != 0) // Looping through the event queue
                    {
                        if (e.type == SDL_QUIT) // Checking if the current event on the poll is QUIT, if it is set quit to true to end the program
                        {
                            quit = true;
                            paused = false;
                        }
                        else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_p)
                            paused = false;
                            Time.reset();
                    }
                }

                Game::player.input(Time.deltaTime(), playerJumpCooldown);
                // Check if the player is close enough to spawn the next biome
                if (Game::player.yPos() > biomeTop+480)
                {
                    // Readjusting the top of the current biome
                    biomeTop += 960;
                    // Setting the previous Biome
                    prevBiome = curBiome;
                    // Randomizing the next biome
                    curBiome = Game::RandBiome(biomes, totBiomes);
                    curBiome.setSeed(time(NULL));
                    curBiome.load(biomeTop);
                }
                // Check if the trails need to be refreshed
                if (Game::player.yPos() > trailPos + SCREEN_HEIGHT-32)
                {
                    SDL_SetRenderTarget(Game::Renderer, prevtrail);
                    SDL_RenderCopy(Game::Renderer, trail, NULL, NULL);
                    SDL_SetRenderTarget(Game::Renderer, trail);
                    SDL_RenderClear(Game::Renderer);
                    SDL_SetRenderTarget(Game::Renderer, NULL);
                    trailPos += SCREEN_HEIGHT;
                }

                // Moving the player
                Game::player.movePlayer(Time.deltaTime());
                // Adding to the player's score
                Game::player.adjustScore(Time.deltaTime());
                // Check for player collision
                if (Game::player.collided())
                {
                    Game::player.crash(crashTimer, Time.deltaTime());
                }

                // Adjusting the player to the camera
                if (Game::player.IsInAir())
                    camera.y = (Game::player.yPos() + 16) - 240;
                else
                    camera.y = (Game::player.yPos() + 16) - 250;

                Time.reset(); // Reset the frame time
                //Drawing to the screen
                SDL_RenderClear(Game::Renderer); // Clearing the screen
                Game::drawTrails(trailTexture, trail, prevtrail, trailPos, camera);
                Game::draw(curBiome, camera, biomeTop, prevBiome, Time.deltaTime());
                SDL_RenderPresent(Game::Renderer);

            if (Game::player.isDead() && !quit)
            {
                Game::GameOver(quit);
                biomeTop = 0;
                trailPos = 0;
                curBiome = Game::RandBiome(biomes, totBiomes);
                curBiome.setSeed(time(NULL));
                curBiome.load(biomeTop);
                SDL_SetRenderTarget(Game::Renderer, trail);
                SDL_SetRenderDrawColor(Game::Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(Game::Renderer);
                SDL_SetRenderTarget(Game::Renderer, NULL);

                SDL_SetRenderTarget(Game::Renderer, prevtrail);
                SDL_SetRenderDrawColor(Game::Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(Game::Renderer);
                SDL_SetRenderTarget(Game::Renderer, NULL);
                prevBiome = curBiome;
                Time.reset();
            }

            }

        }
    }
    // Quiting SDL
    Game::close();
    return 0;
}
