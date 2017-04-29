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
#include "player.h"
#include "biome.h"
#include "obstacle.h"

using namespace std;

const int SCREEN_HEIGHT = 640;
const int SCREEN_WIDTH = 900;


//TODO: Get rid of these globals...
SDL_Window* Window = NULL;
SDL_Renderer* Renderer = NULL;
SDL_Texture* PlayerSpriteSheet;
SDL_Texture* Obstacles;
SDL_Texture* lives;
TTF_Font*font = NULL;
TTF_Font*largeFont = NULL;

Player player;

// Initializing the game
bool init()
{
    // Flag to check if initialization was succesful
    bool goodInit = true;

    // Initializing SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "The SDL could not be initialized! Error: " << SDL_GetError() << endl;
        goodInit = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            cout << "Warning: Linear texture filtering not enabled!";
        }

        // Creating the window
        Window = SDL_CreateWindow("Super Snowboarding", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (Window == NULL)
        {
            cout << "The window could be be created! SDL_Error: " << SDL_GetError() << endl;
            goodInit = false;
        }
        else
        {
            Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
            if (Renderer == NULL)
            {
                cout << "Error creating the renderer! SDL_Error: " << SDL_GetError();
                goodInit = false;
            }
            else
            {
                SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
            //Initialize SDL_ttf
            if( TTF_Init() == -1 )
            {
                cout << "SDL_ttf could not initialize! SDL_ttf Error:" << TTF_GetError() << endl;
                goodInit = false;
            }
        }
    }

    return goodInit; // Returrn whether or not the initialization was successful
}

// Loading textures (as apposed to surfaces)
SDL_Texture* loadTexture(string path)
{
    SDL_Texture* newTexture = NULL; // Used to store the texture

    SDL_Surface* loadedImage = SDL_LoadBMP(path.c_str()); // Loading the image
    // Check for null
    if (loadedImage == NULL)
    {
        cout << "The image could not be loaded!" << SDL_GetError();
    }
    else
    {
        // colorKey the image (set transparency)
        SDL_SetColorKey( loadedImage, SDL_TRUE, SDL_MapRGB( loadedImage->format, 0xFF, 0, 0xFF ) );
        // Creating the texture from the loaded surface
        newTexture = SDL_CreateTextureFromSurface(Renderer, loadedImage);
        if (newTexture == NULL)
        {
            cout << "Unable to create a texture from " << path << " /// SDL_Error: " << SDL_GetError();
        }

        // Freeing the surface from memory
        SDL_FreeSurface(loadedImage);
    }

    return newTexture;
}

// Load all the ressources
bool loadMedia()
{
    // Flag to check if loading was successful
    bool goodLoad = true;

    // Loading the player spritesheet
    PlayerSpriteSheet = loadTexture("../images/playerSpriteSheet.bmp");
    // Check whether or not loading the image was successful
    if (PlayerSpriteSheet == NULL)
    {
        cout << "There was an error loading the image! SDL_Error: " << SDL_GetError() << endl;
        goodLoad = false;
    }

    Obstacles = loadTexture("../images/ObstacleSpriteSheet.bmp");
    if (Obstacles == NULL)
    {
        cout << "There was an error loading the image! SDL_Error: " << SDL_GetError() << endl;
        goodLoad = false;
    }

    font = TTF_OpenFont("../images/font.ttf", 20);
    if (font == NULL)
    {
        cout << "Error loading font! SDL_Error: " << TTF_GetError() << endl;
        goodLoad = false;
    }

    largeFont = TTF_OpenFont("../images/font.ttf", 40);
    if (largeFont == NULL)
    {
        cout << "Error loading font! SDL_Error: " << TTF_GetError() << endl;
        goodLoad = false;
    }

    lives = loadTexture("../images/Life.bmp");
    if (lives == NULL)
    {
        cout << "There was an error loading the LIFE image! SDL_Error: " << SDL_GetError() << endl;
        goodLoad = false;
    }


    return goodLoad;
}

// Quitting SDL
void close()
{
    // Freeing the textures from memory
    SDL_DestroyTexture(PlayerSpriteSheet);
    PlayerSpriteSheet = NULL;

    SDL_DestroyTexture(Obstacles);
    Obstacles = NULL;

    SDL_DestroyTexture(lives);
    lives = NULL;

    TTF_CloseFont(font);
    font = NULL;
    TTF_Quit();

    // Destroying the window
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    Renderer = NULL;
    Window = NULL;

    // Quitting SDL
    SDL_Quit();
}

void drawTrails(SDL_Texture * src, SDL_Texture * curdst, SDL_Texture * prevdst, int yPos, SDL_Rect &camera)
{
    // DRAWING THE TRAILS
    if (!player.IsInAir())
    {
        SDL_SetRenderTarget(Renderer, curdst); // Setting the render target to the background

        SDL_Rect trailRect = {player.xPos() - camera.x, player.yPos()-yPos+24, 32, 8}; // Calculating where the trail should be drawn
        SDL_RenderCopy(Renderer, src, NULL, &trailRect); // Drawing the trail to the background
        SDL_SetRenderTarget(Renderer, NULL); // Resetting the render target to the normal renderer
    }
    // Draw the trails
    SDL_Rect prevBackgroundRect = {0, yPos - SCREEN_HEIGHT - camera.y, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect backgroundRect = {0, yPos - camera.y, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(Renderer, prevdst, NULL, &prevBackgroundRect); // Draw the previous trail
    SDL_RenderCopy(Renderer, curdst, NULL, &backgroundRect); // Draw the texture with the trail drawn onto it to the screen
}
// Drawing everything to the screen
void draw(Biome curBiome, SDL_Rect camera, int biomeTop, Biome prevBiome, float deltaTime)
{
    // Drawing to the screen
    // DRAWING THE BACKGROUND
    prevBiome.spawnToBackground(camera, deltaTime, player, Renderer);
    curBiome.spawnToBackground(camera, deltaTime, player, Renderer);

    player.draw(camera, Renderer, PlayerSpriteSheet);

    // FORGROUND
    prevBiome.spawnToForeground(camera, deltaTime, player, Renderer);
    curBiome.spawnToForeground(camera, deltaTime, player, Renderer);

    // Rendering the player's lives and score
    player.renderLives(Renderer, lives);
    player.renderScore(Renderer, font);



    if (player.collided())
    {
        curBiome.setSeed(time(NULL));
    }
}
void RenderText(int x, int y, string text, SDL_Color color, TTF_Font * fnt)
{
    // Displaying message on SDL window
    SDL_Surface* txt = TTF_RenderText_Solid(fnt, text.c_str(), color);
    SDL_Texture* txture = SDL_CreateTextureFromSurface(Renderer, txt);
    SDL_Rect dst = {x, y, txt->w, txt->h};
    SDL_RenderCopy(Renderer, txture, NULL, &dst);

    SDL_FreeSurface(txt);
    SDL_DestroyTexture(txture);

}
// Choosing a random Biome
Biome RandBiome(Biome biomes[], int amountOfBiomes)
{
    srand(time(NULL));
    int randIndex = rand() % amountOfBiomes;
    for (int x = 0; x < amountOfBiomes; x++)
    {
        if (x == randIndex)
        {
            return biomes[x];
        }
    }

    cout << "There was an error generating the Biome to load!" << endl;
}

void MainMenu()
{
  //  RenderText()

    SDL_RenderPresent(Renderer);
}

void GameOver(bool& endgame)
{

    struct highScore
    {
        string score;
        string name;
    };

    // Displaying message on SDL window
    SDL_RenderClear(Renderer);
    SDL_Color textColour = {0, 0, 0};

    RenderText(SCREEN_WIDTH/2 - 110, 20, "GAME OVER YOU LOSE", textColour, largeFont);
    textColour = {255, 0, 0};
    RenderText(SCREEN_WIDTH/2 - 62, 50, "HIGH SCORES", textColour, largeFont);

    SDL_Color textColor = {0, 0, 0, 0xFF};

    SDL_Event e;

    string inputText = "Rider";
    SDL_RenderPresent(Renderer);
    bool gettingInput = true;


        string name;
        vector<string> lines;

        ifstream inputFile;
        inputFile.open("highscores.txt");
        int tempx = 0;

        while (inputFile.good())
        {
            if (tempx>19)
                break;
            string temp;
            getline(inputFile, temp);
            if (temp != "")
            {
                lines.push_back(temp);
                tempx++;
            }
        }
        inputFile.close();

        vector<highScore> scores;
        for (int x = 0; x < lines.size()/2; x++)
        {
            scores.push_back(highScore());
        }

        // Getting the scores and names into an array
        for (int x = 0; x < lines.size(); x++)
        {
            if (x < lines.size()/2)
            {
                scores[x].score = lines[x];
            }
            else
                scores[x-lines.size()/2].name = lines[x];
        }

        string playerScore;
        string playerName;
        bool top10 = false;

        // Converting player's score to string
        stringstream scoress;
        scoress << player.getScore();
        scoress >> playerScore;

        for (int x = 0; x < scores.size(); x++)
        {
            stringstream ss;
            ss << scores[x].score;
            int score1;
            ss >> score1;

            if (player.getScore() > score1 || scores.size() < 10)
            {
                top10 = true;
                scores.push_back(highScore());
                scores[scores.size()-1].score = playerScore;
                break;
            }
        }

        if (scores.size() == 0)
        {
            scores.push_back(highScore());
            scores[0].score = playerScore;
            top10 = true;
        }


    if (top10)
    {
    SDL_StartTextInput();
    while (gettingInput)
    {
        SDL_RenderClear(Renderer);
        textColour={0, 0, 0};
        RenderText(SCREEN_WIDTH/2 - 110, 20, "GAME OVER YOU LOSE!", textColour, largeFont);
        textColour={255, 0, 0};
        RenderText(SCREEN_WIDTH/2 - 230, 50, "New high score! Please enter your name:", textColour, largeFont);

        while (SDL_PollEvent(&e) != 0)
        {

            if (e.type == SDL_QUIT) // Checking if the current event on the poll is QUIT, if it is set quit to true to end the program
            {
                gettingInput = false;
                endgame = true;
                return;
            }
            else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0 )
            {
                //lop off character
                inputText.pop_back();
            }
            else if( e.type == SDL_TEXTINPUT && inputText.length() < 8)
            {
                //Append character
                inputText += e.text.text;
            }
            else if(e.key.keysym.sym == SDLK_RETURN)
                gettingInput = false;
        }
        if (inputText != "")
            RenderText(SCREEN_WIDTH/2-13, 95, inputText.c_str(), textColor, font);
        else
            RenderText(SCREEN_WIDTH/2-13, 95, " ", textColor, font);
        SDL_RenderPresent(Renderer);
    }
        scores[scores.size()-1].name = inputText;
    SDL_StopTextInput();
    }
    // Sorting the scores
       for (int x = 0; x < scores.size(); x++)
        {
            for (int y = 0; y < scores.size(); y++)
            {
                //Converting Score into int instead of string
                stringstream ss;
                ss << scores[x].score;
                int score1;
                ss >> score1;

                stringstream ss2;
                ss2 << scores[y].score;
                int score2;
                ss2 >> score2;
                if (score1 > score2)
                {
                    highScore temp = scores[x];
                    scores[x] = scores[y];
                    scores[y] = temp;
                }
            }
        }
    // Showing the scores
    for (int x = 0; x < scores.size(); x++)
    {
        string output = scores[x].name;
        RenderText(SCREEN_WIDTH/2 - 60, x*22 + 125, output.c_str(), textColor, font);
        output = scores[x].score;
        RenderText(SCREEN_WIDTH/2 +20, x*22 + 125, output.c_str(), textColor, font);
    }
    SDL_RenderPresent(Renderer);
    while (scores.size() > 10)
    {
        scores.pop_back();
    }

        ofstream outputFile;
        outputFile.open("highscores.txt");
        int i = 0;
        for (int x = 0; x < scores.size()*2; x++)
        {
            if (x < scores.size())
            {
                outputFile << scores[x].score;
            }
            else
            {
                outputFile << scores[i].name;
                i++;
            }

            if (x <  scores.size()*2-1)
            {
                outputFile << endl;
            }

        }
        outputFile.close();
        RenderText(SCREEN_WIDTH/2 - 95, SCREEN_HEIGHT - 40, "Press Enter to Retry, ESC to quit.", textColor, font);
        SDL_RenderPresent(Renderer);
        bool exit = false;
        while(!exit)
        {

             while (SDL_PollEvent(&e) != 0)
            {

                if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
                {
                    exit = true;
                }
                else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE || e.type== SDL_QUIT )
                {
                    endgame = true;
                    exit = true;
                }
            }
    }
    player.reset();


}

/*************************
The main game function
*************************/
int main(int argc, char* args[])
{
    if (!init())
    {
        cout << "Failed to initialize!\n";
    }
    else
    {
        // Load media
        if (!loadMedia())
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
            Obstacle cabin("cabin", 0, 0, 128, 128, 20, 70, 88, 48, false, false, 0, Obstacles);
            Obstacle stairs("stairs", 128, 0, 64, 128, 10, 15, 50, 98, false, true, 0, Obstacles);
            Obstacle tree("tree", 192, 0, 64, 64, 28, 50, 3, 2, false, false, 0, Obstacles);
            Obstacle rock("rock", 0, 128, 32, 32, 10, 20, 9, 8, true, false, 25, Obstacles);
            Obstacle bigRock("big rock", 384, 0, 64, 64, 25, 25, 29, 32, false, false, 0, Obstacles);
            Obstacle cliff("cliff", 256, 0, 128, 128, 15, 50, 98, 78, true, true, 60, Obstacles);
            Obstacle jump("jump", 0, 160, 64, 64, true, Obstacles);
            Obstacle log ("log", 192, 128, 64, 32, 7, 5, 50, 15, true, false, 25, Obstacles);

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
            Biome curBiome = RandBiome(biomes, totBiomes);


            // Initializing the trails
            SDL_Texture* trailTexture = loadTexture("../images/trail.bmp"); // Loading the trail texture
            // Creating the background textures to draw the trail to
            SDL_Texture * trail = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ABGR1555, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
            // Setting it to be white
            SDL_SetRenderTarget(Renderer, trail);
            SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(Renderer);
            SDL_SetRenderTarget(Renderer, NULL);
            // Creating the background texture to store the previous trails
            SDL_Texture * prevtrail = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ABGR1555, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
            // Setting it to be white
            SDL_SetRenderTarget(Renderer, prevtrail);
            SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(Renderer);
            SDL_SetRenderTarget(Renderer, NULL);

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
                        RenderText(SCREEN_WIDTH/2-18, SCREEN_HEIGHT/2, "PAUSED", defaultTextColor, font);
                        SDL_RenderPresent(Renderer);
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

                player.input(Time.deltaTime(), playerJumpCooldown);
                // Check if the player is close enough to spawn the next biome
                if (player.yPos() > biomeTop+480)
                {
                    // Readjusting the top of the current biome
                    biomeTop += 960;
                    // Setting the previous Biome
                    prevBiome = curBiome;
                    // Randomizing the next biome
                    curBiome = RandBiome(biomes, totBiomes);
                    curBiome.setSeed(time(NULL));
                    curBiome.load(biomeTop);
                }
                // Check if the trails need to be refreshed
                if (player.yPos() > trailPos + SCREEN_HEIGHT-32)
                {
                    SDL_SetRenderTarget(Renderer, prevtrail);
                    SDL_RenderCopy(Renderer, trail, NULL, NULL);
                    SDL_SetRenderTarget(Renderer, trail);
                    SDL_RenderClear(Renderer);
                    SDL_SetRenderTarget(Renderer, NULL);
                    trailPos += SCREEN_HEIGHT;
                }

                // Moving the player
                player.movePlayer(Time.deltaTime());
                // Adding to the player's score
                player.adjustScore(Time.deltaTime());
                // Check for player collision
                if (player.collided())
                {
                    player.crash(crashTimer, Time.deltaTime());
                }

                // Adjusting the player to the camera
                if (player.IsInAir())
                    camera.y = (player.yPos() + 16) - 240;
                else
                    camera.y = (player.yPos() + 16) - 250;

                Time.reset(); // Reset the frame time
                //Drawing to the screen
                SDL_RenderClear(Renderer); // Clearing the screen
                drawTrails(trailTexture, trail, prevtrail, trailPos, camera);
                draw(curBiome, camera, biomeTop, prevBiome, Time.deltaTime());
                SDL_RenderPresent(Renderer);

            if (player.isDead() && !quit)
            {
                GameOver(quit);
                biomeTop = 0;
                trailPos = 0;
                curBiome = RandBiome(biomes, totBiomes);
                curBiome.setSeed(time(NULL));
                curBiome.load(biomeTop);
                SDL_SetRenderTarget(Renderer, trail);
                SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(Renderer);
                SDL_SetRenderTarget(Renderer, NULL);

                SDL_SetRenderTarget(Renderer, prevtrail);
                SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(Renderer);
                SDL_SetRenderTarget(Renderer, NULL);
                prevBiome = curBiome;
                Time.reset();
            }

            }

        }
    }
    // Quiting SDL
    close();
    return 0;
}
