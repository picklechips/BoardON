// All sources are created and copyrighted by Ryan Martin - 2014.
// Do not use or alter any of the files without written consent.

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <iomanip>
#include <ctime> // Used to seed srand();
#include <stdlib.h> // used to generate random numbers
#include <sstream>
#include <fstream> // Used for saving highscores
using namespace std;
/*********************************
        DEFINING CLASSES
**********************************/

const float jumpTime = 0.3f;

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

// Obstacle class
class Obstacle
{
private:
    bool canCollide; // Can this obstacle collide with the player?
    SDL_Rect collisionRect;
    bool jumpable;
    bool isBgOnly;
    bool jumpedOver;
    float pointsToGive;
    string name;

protected:
    int xPos, yPos;
    SDL_Rect imgRect;

public:
    Obstacle();
    Obstacle(string name, int x, int y, int height, int width, bool isBgOnly); // Obstacle without collision
    Obstacle(string name, int x, int y, int height, int width, int collisionx, int collisiony, int collisionWidth, int collisionHeight, bool jumpable, bool isBgOnly, float ptstogive); // Obstacle with collision
    void drawObstacle(int x, int y, SDL_Rect camera, float deltaTime); // Drawing the obstacle to the screen
    virtual void drawToBackground(int x, int y, SDL_Rect camera, float deltaTime); // Drawing it to the screen after the player passed
    virtual void drawToForeground(int x, int y, SDL_Rect camera, float deltaTime); // Drawing it to the screen before the player has passed
    void setxy(int x, int y){xPos = x;yPos = y;}
    int getxPos(){return xPos;};
    int getyPos(){return yPos;};
    int getWidth(){return imgRect.w;};
    int getHeight(){return imgRect.h;};
    bool isJumpable(){return jumpable;}
};
// Player Class
class Player
{
    // Declaring the private members
private:
    float x, y; // The position of the player
    int yVelocity, xVelocity; // The velocity of the player
    SDL_Rect curSprite;
    bool isAirborne; // Check if the player is in the air
    float airborneTimer = 0;
    bool collision; // Check if the player collided
    int livesRemaining; // The amount of lives
    // KEEPING TRACK OF SCORE
    int score;
    int scoreMultiplier;
    float pointsToNextMultiplier;
    float scoreTimer; // Used to check when to add to the players score
    float timeSinceLastMultiply;
    Obstacle collidedObst;
    bool dead;
public:
    // Constructor
    Player();
    const int SPEED = 200;
    const int HEIGHT = 32; // Height of the player
    const int WIDTH = 32; // Width of the player
    int xPos() {return (int)x;};// Getting the players x position
    int yPos() {return (int) y;};// Getting the players y position
    bool collided(){return collision;}; // Check if player collided
    void setCollision(bool hasCollided){collision = hasCollided;};
    int yVel(){return yVelocity;} // Getting the players yVelocity
    bool IsInAir(){return isAirborne;} // Checking if the player is in the air
    void input(float deltaTime, float &jumpCooldown); // Taking player input
    void draw(SDL_Rect camera); // Drawing the player to the screen
    void movePlayer(float deltaTime); // Moving the player
    void crash(float &crashTime, float deltaTime); // Dealing with the player after they crashed
    void forceAirborne(float airtime, float velocity);
    void obstCollidedWith(Obstacle obst){collidedObst = obst;} // setting the obstacle that the player collided with
    void adjustScore(float deltaTime);
    void adjustMultiplier(float multiple);
    void renderLives();
    void renderScore();
    bool isDead(){return dead;}
    int getScore(){return score;}
    void reset();
};

// Biome class, allowing for varying environments
class Biome
{
private:
    vector<Obstacle> obstsInBiome; // Store the potential obstacles in the biome
    vector<int> amountOfObst; // The max amount of an obstacle, works respectively with obstsInBiome
    double seed;
public:
    Biome() {};
    Biome(vector<Obstacle> obsts, vector<int> amntObsts);
    ~Biome();
    void load(int bottomOfBiome);
    void spawnToBackground(SDL_Rect camera, float deltaTime); // Draw the biome to the background (after player passes)
    void spawnToForeground(SDL_Rect camera, float deltaTime); // Draw the biome to the foreground (before the player passes)
    void setSeed(double newSeed)
    {seed = newSeed;} // Setting the seed
};

/*********************************
    DEFINING GLOBAL VARIABLES
**********************************/
bool init(); // Starts up SDL and creates window
bool loadMedia(); // Loads up media
void close(); // frees media and shuts down SDL
void drawTrails(SDL_Texture &dst, SDL_Texture &src, SDL_Texture * prevdst, int yPos); // Drawing the trails
void draw(Biome curBiome, SDL_Rect camera, Biome prevBiome); // Draw everything to the screen
SDL_Rect Rect(int x, int y, int width, int height); // Used to set a rect value
Biome RandBiome(Biome biomes[], int amount);
SDL_Texture* loadTexture(string path); // Loads surface as texture

const int SCREEN_HEIGHT = 640;
const int SCREEN_WIDTH = 900;
SDL_Window* Window = NULL; // The window to render to
SDL_Renderer* Renderer = NULL; // The renderer used to render images to the screen
SDL_Texture* PlayerSpriteSheet; // The spritesheet containing the players animations
SDL_Texture* Obstacles; // The sprite sheet of all the obstacles
SDL_Texture* lives;
TTF_Font*font = NULL;
TTF_Font*largeFont = NULL;

// Creating the player!
Player player;

/**********************************
Defining the timer class functions
***********************************/

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
/**********************************
Defining the Player class functions
***********************************/

// Initializing the variables
Player::Player()
{
    x = SCREEN_WIDTH/2 - 16;
    y = 0;
    livesRemaining = 3;
    isAirborne = false;
    xVelocity = 0;
    yVelocity = SPEED;
    score = 0;
    scoreMultiplier = 1;
    SDL_Color textColour = {0, 0, 0, 0};
}

void Player::reset()
{
    x = SCREEN_WIDTH/2 - 16;
    y = 0;
    livesRemaining = 3;
    isAirborne = false;
    xVelocity = 0;
    yVelocity = SPEED;
    score = 0;
    scoreMultiplier = 1;
    SDL_Color textColour = {0, 0, 0, 0};
    dead = false;
}
// Handling the player input
void Player::input(float deltaTime, float &jumpCooldown)
{
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    if (collision)
        airborneTimer = 0;
    if (keyState[SDL_SCANCODE_SPACE] && airborneTimer <= 0 && jumpCooldown > 0.2 && !collision && !isAirborne) // If the player presses the space key
    {
        airborneTimer = jumpTime;
        y -= 10;
        isAirborne = true;
    }
    if (isAirborne)
    {
        airborneTimer -= deltaTime/1000;
        curSprite = Rect(224, 0, 32, 64);
        if (airborneTimer <= 0)
        {
            jumpCooldown = 0;
            airborneTimer = 0;
            y += 10;
            isAirborne = false;
        }
    }
    // Waiting half a second to jump again
    if (jumpCooldown < 0.2 && !isAirborne)
    {
        jumpCooldown += deltaTime/1000;
    }

    if (keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP]) // If the W key is pressed
    {
        if (!isAirborne)
        {
            yVelocity = SPEED/3; // set the y velocity
            curSprite = Rect(32, 0, 32, 32); // Show the player's correct animation
        }
        else
            curSprite = Rect(320, 0, 32, 64);

        // Check for other key presses
        if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && player.xPos()>-6) // if the A key is pressed with the W key
        {
            xVelocity = -SPEED; // Set the x velocity
        }
        else if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT]) && player.xPos()+32 < SCREEN_WIDTH+7) // if the D key is pressed with the W key
        {
            xVelocity = SPEED; // Set the x velocity
        }
        else
            xVelocity = 0;
    }
    else if (keyState[SDL_SCANCODE_S] || keyState[SDL_SCANCODE_DOWN])
    {
        // If the player is airborne show the jump sprite instead
        if (!isAirborne)
        {
            curSprite = Rect(64, 0, 32, 32);
            yVelocity = SPEED*1.5;
        }

        // Check for other key presses
        if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && player.xPos()>-6) // if the A key is pressed with the W key
            xVelocity = -SPEED/2; // Set the x velocity

        else if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT])&& player.xPos()+32 < SCREEN_WIDTH+7) // if the D key is pressed with the W key
            xVelocity = SPEED/2; // Set the x velocity
        else
            xVelocity = 0;
    }
    else if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT])&& player.xPos()+32 < SCREEN_WIDTH+7)
    {
        yVelocity = SPEED;
        xVelocity = SPEED;
        if (isAirborne)
        {
            curSprite = Rect(256, 0, 32, 64);
        }
        else
            curSprite = Rect(160, 0, 32, 32);
    }
    else if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && player.xPos()>-6)
    {
        yVelocity = SPEED;
        xVelocity = -SPEED;
        if (isAirborne)
        {
            curSprite = Rect(288, 0, 32, 64);
        }
        else
            curSprite = Rect(128, 0, 32, 32);
    }
    else if (!isAirborne)
    {
        // Resetting the variables each frame
        curSprite = Rect(0, 0, 32, 32);
        xVelocity = 0;
        yVelocity = SPEED;
    }
    else
    {
        xVelocity = 0;
    }
}

void Player::forceAirborne(float airtime, float velocity)
{
    if (!isAirborne)
        y-=10;
    isAirborne = true;
    yVelocity=velocity;
    airborneTimer=airtime;
}
// Adjusting the player's coordinates
void Player::movePlayer(float deltaTime)
{
    if (!collision)
    {
        x += xVelocity * deltaTime/1000.f;
        y += yVelocity * deltaTime/1000.f;
    }
    //if (y > 480)
    //y = -20;
}

// Respawn the player if they crashed
void Player::crash(float &crashTime, float deltaTime)
{
    if (crashTime > 2.0 && collision)
    {
        crashTime = 0;
        y = collidedObst.getyPos()+collidedObst.getHeight();
        livesRemaining--;
        scoreMultiplier = 1;
        pointsToNextMultiplier = 0;
        if (livesRemaining==0)
        {
            dead = true;
        }
        collision = false;
    }
    else if (collision)
    {
        crashTime += deltaTime/1000;
    }
    else
    {
        crashTime = 0;
    }
}

void Player::adjustScore(float deltaTime)
{
    // Add to the score based on time, so it's not dependant on framerate
    if (scoreTimer > 0.1 && !collision)
    {
        score += yVelocity/10*scoreMultiplier; // the score is the players yVelocity times the multiplier
        scoreTimer = 0; // Reset the timer
    }
    else if (!collision)
    {
        scoreTimer += deltaTime/1000; // Adding to the timer
    }

    // Adjusting the multipler
    if (pointsToNextMultiplier > 0)
    {
        pointsToNextMultiplier -= 5 *scoreMultiplier/2* deltaTime/1000; // Subtracting the points to next multiplier
        if (pointsToNextMultiplier < 0 && scoreMultiplier >1) // If there's less than 0 points, subtract 1 from the multiplier
        {
            scoreMultiplier--;
            pointsToNextMultiplier = 100;
        }
    }
    if (timeSinceLastMultiply > 0)
    {
        timeSinceLastMultiply -= deltaTime/1000;
    }
    cout << pointsToNextMultiplier << endl;
}

// Adjusting the players multiplier
void Player::adjustMultiplier(float multiple)
{
    // adding to the points to next multiplier
    if (timeSinceLastMultiply <= 0)
        pointsToNextMultiplier += multiple;

    // If the points are more than 100, add to the multiplier
    if (pointsToNextMultiplier > 100 && scoreMultiplier<4)
    {
        scoreMultiplier++;
        pointsToNextMultiplier -= 100;
    }
    else if (scoreMultiplier == 4 && pointsToNextMultiplier > 150)
        pointsToNextMultiplier = 150;
    timeSinceLastMultiply = jumpTime;
}

// Drawing the player to the screen
void Player::draw(SDL_Rect camera)
{

    int playerHeight = 32;
    if (isAirborne)
        playerHeight = 64;

    SDL_Rect drawRect = {(int)x - camera.x, (int)y-camera.y, 32, playerHeight};

    if (collision)
    {
        isAirborne = false;
        curSprite = Rect(192, 0, 32, 32);
        drawRect.y += 16;
    }
    SDL_RenderCopy(Renderer, PlayerSpriteSheet, &curSprite, &drawRect);
}

void Player::renderLives()
{
    for (int x = 0, i = 0; x < livesRemaining; x++, i+=32)
    {
        SDL_Rect dstRect = {i, 0, 32, 32};
        SDL_RenderCopy(Renderer, lives, NULL, &dstRect);
    }
}

void Player::renderScore()
{
    // Drawing the player's score
    // Converting the score into a string
    stringstream ss;
    ss << score;
    string scoreOutput;
    ss >> scoreOutput;

    scoreOutput = "SCORE: " + scoreOutput;

    // Converting multipler to string
    stringstream ss1;
    ss1 << scoreMultiplier;
    string multiplier;
    ss1 >> multiplier;

    multiplier = "Multiplier: " + multiplier;
    SDL_Color textColour = {0, 0, 0};
    // Displaying the score
    SDL_Surface * tempSurface = TTF_RenderText_Solid(font, scoreOutput.c_str(), textColour);
    SDL_Texture * scoreTexture = SDL_CreateTextureFromSurface(Renderer, tempSurface);
    SDL_Rect dstRect = {100, 0, tempSurface->w, tempSurface->h};
    SDL_RenderCopy(Renderer, scoreTexture, NULL, &dstRect);

    // Displaying the multiplier
    SDL_Surface * tempSurface2 = TTF_RenderText_Solid(font, multiplier.c_str(), textColour);
    SDL_Texture * multiplierTexture = SDL_CreateTextureFromSurface(Renderer, tempSurface2);
    dstRect = {100, 15, tempSurface2->w, tempSurface2->h};
    SDL_RenderCopy(Renderer, multiplierTexture, NULL, &dstRect);

    // Freeing the surface
    SDL_FreeSurface(tempSurface);
    SDL_FreeSurface(tempSurface2);
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(multiplierTexture);
}
/**********************************
DEFINING OBSTACLE CLASS FUNCTIONS
***********************************/
// Default obstacle constructor
Obstacle::Obstacle()
{
    SDL_Rect tempRect = {32, 32, 32, 32};
    imgRect = tempRect;
    canCollide = false;
    isBgOnly = false;
    jumpedOver = false;
}
// Initializing obstacle WITHOUT collision
Obstacle::Obstacle(string name, int x, int y, int width, int height, bool bgOnly)
{
    SDL_Rect tempRect = {x, y, width, height};
    imgRect = tempRect;
    canCollide = false;
    isBgOnly = bgOnly;
    jumpedOver = false;
}

// Initializing an obstacle WITH collision
Obstacle::Obstacle(string obstname, int x, int y, int width, int height, int collisionx, int collisiony, int collisionWidth, int collisionHeight, bool isJumpable, bool isbg, float ptstogive)
{
    canCollide = true;
    SDL_Rect colRect = {collisionx, collisiony, collisionWidth, collisionHeight};
    SDL_Rect tempRect = {x, y, width, height};
    imgRect = tempRect;
    collisionRect = colRect;
    jumpable = isJumpable;
    isBgOnly = isbg;
    name = obstname;
    jumpedOver = false;
    pointsToGive = ptstogive;
}

void Obstacle::drawObstacle(int x, int y, SDL_Rect camera, float deltaTime)
{
    xPos = x;
    yPos = y;
    // Setting the collision box around the obstacle
    SDL_Rect collideRect = collisionRect;
    collideRect.x = collisionRect.x + x + camera.x;
    collideRect.y = collisionRect.y + y;
    // Setting the rect to draw the image
    SDL_Rect drawRect = {x-camera.x, y-camera.y, imgRect.w, imgRect.h};
    // Drawing the image
    // Only draw it if it's present on the screen
    if (xPos + imgRect.w > camera.x && xPos < SCREEN_WIDTH+camera.x && yPos + imgRect.h > camera.y && yPos < SCREEN_HEIGHT + camera.y)
        SDL_RenderCopy(Renderer, Obstacles, &imgRect, &drawRect);

    // Finding the sides of the player
    int playerLeftSide = player.xPos();
    int playerRightSide = player.xPos() + player.WIDTH;
    int playerTop = player.yPos();
    int playerBottom = player.yPos() + player.HEIGHT;

    // Finding the sides of the obstacle
    int obstLeftSide = collideRect.x;
    int obstRightSide = collideRect.x + collideRect.w;
    int obstTop = collideRect.y;
    int obstBottom = collideRect.y + collideRect.h;

    // If the player collided, check where he will respawn
    int playerYRespawn =  y+imgRect.h;

    if (name == "cliff" && player.yPos()+32 > y+20 && playerLeftSide < obstRightSide-3 && playerRightSide > obstLeftSide+3 && playerTop < y+114)
    {
       if (player.yVel()<= player.SPEED/3)
            player.forceAirborne(0.05, player.SPEED/2.5f);
        else
            player.forceAirborne(0.05, player.SPEED*1.5);
    }
    //Checking for collision (Checking if the player and the obstacle overlap)
    if (!player.collided() && (playerBottom-20 < obstBottom) && canCollide && playerRightSide > obstLeftSide && playerLeftSide < obstRightSide && playerTop < obstBottom && playerBottom > obstTop && ((jumpable && !player.IsInAir()) || !jumpable))
    {
        player.obstCollidedWith(*this);
        player.setCollision(true);
    }
    else if ((!player.collided() && (playerBottom-20 < obstBottom) && canCollide && playerRightSide > obstLeftSide && playerLeftSide < obstRightSide && playerTop < obstBottom && playerBottom > obstTop && ((jumpable && player.IsInAir()))))
    {
        player.adjustMultiplier(pointsToGive);

    }
    // Make sure if the player is trying to respawn he doesn't respawn on another object
    else if (player.collided() && canCollide && playerRightSide > obstLeftSide && playerLeftSide < obstRightSide && playerYRespawn < obstBottom && playerYRespawn+32 > obstTop)
    {
        player.obstCollidedWith(*this);
    }
}

/*
Since After the player passes an obstacle, he is in front of it, the obstacle should be drawn to the
background (behind the player). But if the player has not yet passed the object, it should be
drawn to the foreground (in front of the player). To avoid drawing an object twice each frame (once
to the background and once to the foreground) Two different functions are used, so an object is only
drawn to the screen once. drawToBackground is called before the player is drawn, and drawToForeground
is called AFTER the player is drawn.
*/
void Obstacle::drawToBackground(int x, int y, SDL_Rect camera, float deltaTime)
{
    // Only draw if the player has passed the obstacle
    if (player.yPos()+22 >= y + imgRect.h || ((jumpable && player.IsInAir()) && player.yPos()+22 < y +imgRect.h) || isBgOnly)
    {
        drawObstacle(x, y, camera, deltaTime);
    }
}

void Obstacle::drawToForeground(int x, int y, SDL_Rect camera, float deltaTime)
{
    // Only draw if the player hasn't passed the obstacle
    if (player.yPos()+22 < y + imgRect.h && (!jumpable || (jumpable && !player.IsInAir())) && !isBgOnly)
    {
        drawObstacle(x, y, camera, deltaTime);
    }
}


/**********************************
DEFINING THE BIOME CLASS FUNCTIONS
***********************************/

Biome::Biome(vector<Obstacle> obsts, vector<int> amntObsts)
{
    // Storing the ostacles in a vector
    for (int x = 0; x < obsts.size(); x++)
    {
        for (int y = 0; y < amntObsts[x]; y++)
        {
            // Adding the amount and type of each obstacle to the vector
            obstsInBiome.push_back(obsts[x]);
        }
    }
}
Biome::~Biome()
{
    //delete[]obstsInBiome; // deleting the obstacle array
}

// Loading the Biome
void Biome::load(int topOfBiome)
{
    srand(seed);
    vector <SDL_Rect> obstPositions;
    for (int x = 0; x < obstsInBiome.size(); x++)
    {
        int xLoc = rand() % 1304 - 64; // Randomizing the x location
        int yLoc = rand() % (960 - obstsInBiome[x].getHeight()) + topOfBiome; // Randomizing the y location
        if (x==0)
        {
            xLoc = -1000;
            yLoc = -1000;
        }
        if (yLoc + obstsInBiome[x].getHeight() > 960 + topOfBiome)
        {
            cout << "The obstacle is outside of the chunk!" << endl;
            yLoc -= obstsInBiome[x].getHeight() - (yLoc + obstsInBiome[x].getHeight() - 960 + topOfBiome);
        }
        // Calculating the sides of the obstacle trying to spawn
        int top = yLoc;
        int bottom = yLoc + obstsInBiome[x].getHeight();
        int right = xLoc + obstsInBiome[x].getWidth();
        int left = xLoc;

        for (int i = 0; i < obstPositions.size(); i++)
        {
            // Calculating the sides of the already spawned object
            int spawnedTop = obstPositions[i].y;
            int spawnedBottom = obstPositions[i].y + obstPositions[i].h;
            int spawnedRight = obstPositions[i].x + obstPositions[i].w;
            int spawnedLeft = obstPositions[i].x;
            while (bottom > spawnedTop && right > spawnedLeft && left < spawnedRight && top < spawnedBottom)
            {
                yLoc += obstPositions[i].h - (top - spawnedTop - 10);
                bottom += obstPositions[i].h - (top - spawnedTop - 10);
                top += obstPositions[i].h - (top - spawnedTop - 10);
                i = 0;
            }
        }
        // If the object is pushed too far down, into the next chunk, spawn it off screen to avoid overlapping.
        if (bottom > topOfBiome+960)
        {
            obstsInBiome[x].setxy(-1000, -1000);
        }
        else
        {
            obstPositions.push_back(Rect(xLoc, yLoc, obstsInBiome[x].getWidth(), obstsInBiome[x].getHeight()));
            obstsInBiome[x].setxy(xLoc, yLoc);
        }
    }
}

// Spawning the biome to the foreground (Before the player passes the object)
void Biome::spawnToForeground(SDL_Rect camera, float deltaTime)
{
    for (int x = 0; x < obstsInBiome.size(); x++)
    {
        obstsInBiome[x].drawToForeground(obstsInBiome[x].getxPos(), obstsInBiome[x].getyPos(), camera, deltaTime);
    }
}

// Spawning the biome to the background (After the player passes the object)
void Biome::spawnToBackground(SDL_Rect camera, float deltaTime)
{
    for (int x = 0; x < obstsInBiome.size(); x++)
    {
        obstsInBiome[x].drawToBackground(obstsInBiome[x].getxPos(), obstsInBiome[x].getyPos(), camera, deltaTime);
    }
}
/**********************************
DEFINING THE GLOBAL FUNCTIONS
***********************************/
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

            SDL_Surface* loadedImage = SDL_LoadBMP("Life.bmp"); // Loading the image
            // Check for null
            if (loadedImage == NULL)
            {
                cout << "The image could not be loaded!" << SDL_GetError();
            }
            else
            {
                // colorKey the image (set transparency)
                SDL_SetColorKey( loadedImage, SDL_TRUE, SDL_MapRGB( loadedImage->format, 0xFF, 0, 0xFF ) );
                SDL_SetWindowIcon(Window, loadedImage);
                // Freeing the surface from memory
                SDL_FreeSurface(loadedImage);
            }
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

// Load all the ressources
bool loadMedia()
{
    // Flag to check if loading was successful
    bool goodLoad = true;

    // Loading the player spritesheet
    PlayerSpriteSheet = loadTexture("playerSpriteSheet.bmp");
    // Check whether or not loading the image was successful
    if (PlayerSpriteSheet == NULL)
    {
        cout << "There was an error loading the image! SDL_Error: " << SDL_GetError() << endl;
        goodLoad = false;
    }

    // Loading the obstacles in a spritesheet
    Obstacles = loadTexture("ObstacleSpriteSheet.bmp");
    if (Obstacles == NULL)
    {
        // Displaying the error
        cout << "There was an error loading the image! SDL_Error: " << SDL_GetError() << endl;
        goodLoad = false;
    }

    font = TTF_OpenFont("font.ttf", 20);
    if (font == NULL)
    {
        cout << "Error loading font! SDL_Error: " << TTF_GetError() << endl;
        goodLoad = false;
    }

    largeFont = TTF_OpenFont("font.ttf", 40);
    if (largeFont == NULL)
    {
        cout << "Error loading font! SDL_Error: " << TTF_GetError() << endl;
        goodLoad = false;
    }

    lives = loadTexture("Life.bmp");
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

// Used to set a SDL_Rect without needing to create a temporary rect
SDL_Rect Rect(int x, int y, int width, int height)
{
    SDL_Rect tempRect = {x, y, width, height};
    return tempRect;
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
    prevBiome.spawnToBackground(camera, deltaTime);
    curBiome.spawnToBackground(camera, deltaTime);

    player.draw(camera);

    // FORGROUND
    prevBiome.spawnToForeground(camera, deltaTime);
    curBiome.spawnToForeground(camera, deltaTime);

    // Rendering the player's lives and score
    player.renderLives();
    player.renderScore();



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
            Obstacle cabin("cabin", 0, 0, 128, 128, 20, 70, 88, 48, false, false, 0);
            Obstacle stairs("stairs", 128, 0, 64, 128, 10, 15, 50, 98, false, true, 0);
            Obstacle tree("tree", 192, 0, 64, 64, 28, 50, 3, 2, false, false, 0);
            Obstacle rock("rock", 0, 128, 32, 32, 10, 20, 9, 8, true, false, 25);
            Obstacle bigRock("big rock", 384, 0, 64, 64, 25, 25, 29, 32, false, false, 0);
            Obstacle cliff("cliff", 256, 0, 128, 128, 15, 50, 98, 78, true, true, 60);
            Obstacle jump("jump", 0, 160, 64, 64, true);
            Obstacle log ("log", 192, 128, 64, 32, 7, 5, 50, 15, true, false, 25);

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
            SDL_Texture* trailTexture = loadTexture("trail.bmp"); // Loading the trail texture
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
