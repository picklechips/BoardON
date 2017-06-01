#include <iostream>
#include <iomanip>
#include <ctime> // Used to seed srand();
#include <stdlib.h> // used to generate random numbers
#include <sstream>
#include <fstream> // Used for saving highscores

#include "time.h"
#include "player.h"
#include "obstacle.h"
#include "game.h"

SDL_Window* Game::m_window = NULL;
SDL_Renderer* Game::m_renderer = NULL;
SDL_Texture* Game::m_playerSpriteSheet = NULL;
SDL_Texture* Game::m_obstacleSpriteSheet = NULL;
SDL_Texture* Game::m_livesImg;
TTF_Font* Game::m_defaultFont = NULL;
TTF_Font* Game::m_largeFont = NULL;

Player Game::m_player;

using namespace std;

//Accessors
SDL_Window* Game::Window()
{
    return m_window;
}

SDL_Renderer* Game::Renderer()
{
    return m_renderer;
}

SDL_Texture* Game::PlayerSpriteSheet()
{
    return m_playerSpriteSheet;
}

SDL_Texture* Game::ObstacleSpriteSheet()
{
    return m_obstacleSpriteSheet;
}

SDL_Texture* Game::LivesIMG()
{
    return m_livesImg;
}

TTF_Font* Game::DefaultFont()
{
    return m_defaultFont;
}
TTF_Font* Game::LargeFont()
{
    return m_largeFont;
}

Player& Game::MainPlayer()
{
    return m_player;
}

// Initializing the game
bool Game::Init()
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
        m_window = SDL_CreateWindow("BoardON", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (m_window == NULL)
        {
            cout << "The window could be be created! SDL_Error: " << SDL_GetError() << endl;
            goodInit = false;
        }
        else
        {
            m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
            if (m_renderer == NULL)
            {
                cout << "Error creating the renderer! SDL_Error: " << SDL_GetError();
                goodInit = false;
            }
            else
            {
                SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
            //Initialize SDL_ttf
            if( TTF_Init() == -1 )
            {
                cout << "SDL_ttf could not initialize! SDL_ttf Error:" << TTF_GetError() << endl;
                goodInit = false;
            }
        }
    }

    return goodInit;
}

SDL_Texture* Game::LoadTexture(string path)
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
        newTexture = SDL_CreateTextureFromSurface(m_renderer, loadedImage);
        if (newTexture == NULL)
        {
            cout << "Unable to create a texture from " << path << " /// SDL_Error: " << SDL_GetError();
        }

        // Freeing the surface from memory
        SDL_FreeSurface(loadedImage);
    }

    return newTexture;
}

bool Game::LoadMedia()
{
    // Flag to check if loading was successful
    bool goodLoad = true;

    // Loading the player spritesheet
    m_playerSpriteSheet = LoadTexture("../images/playerSpriteSheet.bmp");
    // Check whether or not loading the image was successful
    if (m_playerSpriteSheet == NULL)
    {
        cout << "There was an error loading the image! SDL_Error: " << SDL_GetError() << endl;
        goodLoad = false;
    }

    m_obstacleSpriteSheet = LoadTexture("../images/ObstacleSpriteSheet.bmp");
    if (m_obstacleSpriteSheet == NULL)
    {
        cout << "There was an error loading the image! SDL_Error: " << SDL_GetError() << endl;
        goodLoad = false;
    }

    m_defaultFont = TTF_OpenFont("../images/font.ttf", 20);
    if (m_defaultFont == NULL)
    {
        cout << "Error loading font! SDL_Error: " << TTF_GetError() << endl;
        goodLoad = false;
    }

    m_largeFont = TTF_OpenFont("../images/font.ttf", 40);
    if (m_largeFont == NULL)
    {
        cout << "Error loading font! SDL_Error: " << TTF_GetError() << endl;
        goodLoad = false;
    }

    m_livesImg = LoadTexture("../images/Life.bmp");
    if (m_livesImg == NULL)
    {
        cout << "There was an error loading the LIFE image! SDL_Error: " << SDL_GetError() << endl;
        goodLoad = false;
    }


    return goodLoad;
}

// Quitting SDL
void Game::Close()
{
    // Freeing the textures from memory
    SDL_DestroyTexture(m_playerSpriteSheet);
    m_playerSpriteSheet = NULL;

    SDL_DestroyTexture(m_obstacleSpriteSheet);
    m_obstacleSpriteSheet = NULL;

    SDL_DestroyTexture(m_livesImg);
    m_livesImg = NULL;

    TTF_CloseFont(m_defaultFont);
    m_defaultFont = NULL;

    TTF_CloseFont(m_largeFont);
    m_defaultFont = NULL;
    TTF_Quit();

    // Destroying the window
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    m_renderer = NULL;
    m_window = NULL;

    // Quitting SDL
    SDL_Quit();
}

void Game::DrawTrails(SDL_Texture * src, SDL_Texture * curdst, SDL_Texture * prevdst, int yPos, SDL_Rect &camera)
{
    // DRAWING THE TRAILS
    if (!m_player.IsInAir())
    {
        SDL_SetRenderTarget(m_renderer, curdst); // Setting the render target to the background

        SDL_Rect trailRect = {m_player.xPos() - camera.x, m_player.yPos()-yPos+24, 32, 8}; // Calculating where the trail should be drawn
        SDL_RenderCopy(m_renderer, src, NULL, &trailRect); // Drawing the trail to the background
        SDL_SetRenderTarget(m_renderer, NULL); // Resetting the render target to the normal renderer
    }
    // Draw the trails
    SDL_Rect prevBackgroundRect = {0, yPos - SCREEN_HEIGHT - camera.y, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect backgroundRect = {0, yPos - camera.y, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(m_renderer, prevdst, NULL, &prevBackgroundRect); // Draw the previous trail
    SDL_RenderCopy(m_renderer, curdst, NULL, &backgroundRect); // Draw the texture with the trail drawn onto it to the screen
}
// Drawing everything to the screen
void Game::Draw(Biome curBiome, SDL_Rect camera, int biomeTop, Biome prevBiome, float deltaTime)
{
    // Drawing to the screen
    // DRAWING THE BACKGROUND
    prevBiome.SpawnToBackground(camera, deltaTime, m_player, m_renderer);
    curBiome.SpawnToBackground(camera, deltaTime, m_player, m_renderer);

    m_player.draw(camera, m_renderer, m_playerSpriteSheet);

    // FORGROUND
    prevBiome.SpawnToForeground(camera, deltaTime, m_player, m_renderer);
    curBiome.SpawnToForeground(camera, deltaTime, m_player, m_renderer);

    // Rendering the player's lives and score
    m_player.renderLives(m_renderer, m_livesImg);
    m_player.renderScore(m_renderer, m_defaultFont);



    if (m_player.collided())
    {
        curBiome.SetSeed(time(NULL));
    }
}
void Game::RenderText(int x, int y, string text, SDL_Color color, TTF_Font * fnt)
{
    // Displaying message on SDL window
    SDL_Surface* txt = TTF_RenderText_Solid(fnt, text.c_str(), color);
    SDL_Texture* txture = SDL_CreateTextureFromSurface(m_renderer, txt);
    SDL_Rect dst = {x, y, txt->w, txt->h};
    SDL_RenderCopy(m_renderer, txture, NULL, &dst);

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
    SDL_RenderClear(m_renderer);
    SDL_Color textColour = {0, 0, 0};

    RenderText(SCREEN_WIDTH/2 - 110, 20, "GAME OVER YOU LOSE", textColour, m_largeFont);
    textColour = {255, 0, 0};
    RenderText(SCREEN_WIDTH/2 - 62, 50, "HIGH SCORES", textColour, m_largeFont);

    SDL_Color textColor = {0, 0, 0, 0xFF};

    SDL_Event e;

    string inputText = "Rider";
    SDL_RenderPresent(m_renderer);
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
        scoress << m_player.getScore();
        scoress >> playerScore;

        for (int x = 0; x < scores.size(); x++)
        {
            stringstream ss;
            ss << scores[x].score;
            int score1;
            ss >> score1;

            if (m_player.getScore() > score1 || scores.size() < 10)
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
            SDL_RenderClear(m_renderer);
            textColour={0, 0, 0};
            RenderText(SCREEN_WIDTH/2 - 110, 20, "GAME OVER YOU LOSE!", textColour, m_largeFont);
            textColour={255, 0, 0};
            RenderText(SCREEN_WIDTH/2 - 230, 50, "New high score! Please enter your name:", textColour, m_largeFont);

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
                RenderText(SCREEN_WIDTH/2-13, 95, inputText.c_str(), textColor, m_defaultFont);
            else
                RenderText(SCREEN_WIDTH/2-13, 95, " ", textColor, m_defaultFont);
            SDL_RenderPresent(m_renderer);
        }
        if (inputText == "")
            inputText = " ";
        
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
        RenderText(SCREEN_WIDTH/2 - 60, x*22 + 125, output.c_str(), textColor, m_defaultFont);
        output = scores[x].score;
        RenderText(SCREEN_WIDTH/2 +20, x*22 + 125, output.c_str(), textColor, m_defaultFont);
    }
    SDL_RenderPresent(m_renderer);
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
        RenderText(SCREEN_WIDTH/2 - 95, SCREEN_HEIGHT - 40, "Press Enter to Retry, ESC to quit.", textColor, m_defaultFont);
        SDL_RenderPresent(m_renderer);
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

    m_player.reset();
}