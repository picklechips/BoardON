#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <sstream>

#include "player.h"

using namespace std;

const float jumpTime = 0.3f;
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 640;
SDL_Rect Rect(int x, int y, int width, int height)
{
    SDL_Rect tempRect = {x, y, width, height};
    return tempRect;
}

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

void Player::obstCollidedWith(int pos, int height)
{
    collidedObst.yPos = pos;
    collidedObst.height = height;
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
        if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && x >-6) // if the A key is pressed with the W key
        {
            xVelocity = -SPEED; // Set the x velocity
        }
        else if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT]) && x+32 < SCREEN_WIDTH+7) // if the D key is pressed with the W key
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
        if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && x >-6) // if the A key is pressed with the W key
            xVelocity = -SPEED/2; // Set the x velocity

        else if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT])&& x+32 < SCREEN_WIDTH+7) // if the D key is pressed with the W key
            xVelocity = SPEED/2; // Set the x velocity
        else
            xVelocity = 0;
    }
    else if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT])&& x+32 < SCREEN_WIDTH+7)
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
    else if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && x >-6)
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
        y = collidedObst.yPos+collidedObst.height;
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
void Player::draw(SDL_Rect camera, SDL_Renderer* renderer, SDL_Texture *spriteSheet)
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
    SDL_RenderCopy(renderer, spriteSheet, &curSprite, &drawRect);
}

void Player::renderLives(SDL_Renderer* renderer, SDL_Texture *lives)
{
    for (int x = 0, i = 0; x < livesRemaining; x++, i+=32)
    {
        SDL_Rect dstRect = {i, 0, 32, 32};
        SDL_RenderCopy(renderer, lives, NULL, &dstRect);
    }
}

void Player::renderScore(SDL_Renderer* renderer, TTF_Font* font)
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
    SDL_Texture * scoreTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_Rect dstRect = {100, 0, tempSurface->w, tempSurface->h};
    SDL_RenderCopy(renderer, scoreTexture, NULL, &dstRect);

    // Displaying the multiplier
    SDL_Surface * tempSurface2 = TTF_RenderText_Solid(font, multiplier.c_str(), textColour);
    SDL_Texture * multiplierTexture = SDL_CreateTextureFromSurface(renderer, tempSurface2);
    dstRect = {100, 15, tempSurface2->w, tempSurface2->h};
    SDL_RenderCopy(renderer, multiplierTexture, NULL, &dstRect);

    // Freeing the surface
    SDL_FreeSurface(tempSurface);
    SDL_FreeSurface(tempSurface2);
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(multiplierTexture);
}