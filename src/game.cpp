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
#include "obstacle.h"
#include "game.h"

SDL_Window* Game::Window = NULL;
SDL_Renderer* Game::Renderer = NULL;
SDL_Texture* Game::PlayerSpriteSheet = NULL;
SDL_Texture* Game::Obstacles = NULL;
SDL_Texture* Game::lives;
TTF_Font* Game::font = NULL;
TTF_Font* Game::largeFont = NULL;

Player Game::player = Player();

using namespace std;

// Initializing the game
bool Game::init()
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

SDL_Texture* Game::loadTexture(string path)
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

bool Game::loadMedia()
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
void Game::close()
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

void Game::drawTrails(SDL_Texture * src, SDL_Texture * curdst, SDL_Texture * prevdst, int yPos, SDL_Rect &camera)
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
void Game::draw(Biome curBiome, SDL_Rect camera, int biomeTop, Biome prevBiome, float deltaTime)
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
void Game::RenderText(int x, int y, string text, SDL_Color color, TTF_Font * fnt)
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
Biome Game::RandBiome(Biome biomes[], int amountOfBiomes)
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

void Game::GameOver(bool& endgame)
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