#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <sstream>

#include "player.h"
#include "game.h"

using namespace std;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
const float jumpTime = 0.3f;

SDL_Rect Rect(int x, int y, int width, int height)
{
    SDL_Rect tempRect = {x, y, width, height};
    return tempRect;
}

Player::Player() :
    m_x {SCREEN_WIDTH/2 - 16},
    m_y {0},
    m_livesRemaining {3},
    m_isAirborne {false},
    m_xVelocity {0},
    m_yVelocity {SPEED},
    m_score {0},
    m_scoreMultiplier {1}
{
}

int Player::xPos()
{
    return (int)m_x;
}

int Player::yPos()
{
    return (int)m_y;
}

int Player::yVel()
{
    return m_yVelocity;
}

bool Player::collided()
{
    return m_collision;
}

bool Player::IsInAir()
{
    return m_isAirborne;
}

int Player::getScore()
{
    return m_score;
}

bool Player::isDead()
{
    return m_dead;
}

void Player::setCollision(bool hasCollided) 
{
    m_collision = hasCollided;
}

void Player::obstCollidedWith(int pos, int height)
{
    m_collidedObst.yPos = pos;
    m_collidedObst.height = height;
}

void Player::reset()
{
    m_x = SCREEN_WIDTH/2 - 16;
    m_y = 0;
    m_livesRemaining = 3;
    m_isAirborne = false;
    m_xVelocity = 0;
    m_yVelocity = SPEED;
    m_score = 0;
    m_scoreMultiplier = 1;
    m_dead = false;
}
// Handling the player input
void Player::input(float deltaTime, float &jumpCooldown)
{
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    if (m_collision)
        m_airborneTimer = 0;
    if (keyState[SDL_SCANCODE_SPACE] && m_airborneTimer <= 0 && jumpCooldown > 0.2 && !m_collision && !m_isAirborne) // If the player presses the space key
    {
        m_airborneTimer = jumpTime;
        m_y -= 10;
        m_isAirborne = true;
    }
    if (m_isAirborne)
    {
        m_airborneTimer -= deltaTime/1000;
        m_curSprite = Rect(224, 0, 32, 64);
        if (m_airborneTimer <= 0)
        {
            jumpCooldown = 0;
            m_airborneTimer = 0;
            m_y += 10;
            m_isAirborne = false;
        }
    }
    // Waiting half a second to jump again
    if (jumpCooldown < 0.2 && !m_isAirborne)
    {
        jumpCooldown += deltaTime/1000;
    }

    if (keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP]) // If the W key is pressed
    {
        if (!m_isAirborne)
        {
            m_yVelocity = SPEED/3; // set the y velocity
            m_curSprite = Rect(32, 0, 32, 32); // Show the player's correct animation
        }
        else
            m_curSprite = Rect(320, 0, 32, 64);

        // Check for other key presses
        if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && m_x >-6) // if the A key is pressed with the W key
        {
            m_xVelocity = -SPEED; // Set the x velocity
        }
        else if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT]) && m_x+32 < SCREEN_WIDTH+7) // if the D key is pressed with the W key
        {
            m_xVelocity = SPEED; // Set the x velocity
        }
        else
            m_xVelocity = 0;
    }
    else if (keyState[SDL_SCANCODE_S] || keyState[SDL_SCANCODE_DOWN])
    {
        // If the player is airborne show the jump sprite instead
        if (!m_isAirborne)
        {
            m_curSprite = Rect(64, 0, 32, 32);
            m_yVelocity = SPEED*1.5;
        }

        // Check for other key presses
        if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && m_x >-6) // if the A key is pressed with the W key
            m_xVelocity = -SPEED/2; // Set the x velocity

        else if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT])&& m_x+32 < SCREEN_WIDTH+7) // if the D key is pressed with the W key
            m_xVelocity = SPEED/2; // Set the x velocity
        else
            m_xVelocity = 0;
    }
    else if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT])&& m_x+32 < SCREEN_WIDTH+7)
    {
        m_yVelocity = SPEED;
        m_xVelocity = SPEED;
        if (m_isAirborne)
        {
            m_curSprite = Rect(256, 0, 32, 64);
        }
        else
            m_curSprite = Rect(160, 0, 32, 32);
    }
    else if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && m_x >-6)
    {
        m_yVelocity = SPEED;
        m_xVelocity = -SPEED;
        if (m_isAirborne)
        {
            m_curSprite = Rect(288, 0, 32, 64);
        }
        else
            m_curSprite = Rect(128, 0, 32, 32);
    }
    else if (!m_isAirborne)
    {
        // Resetting the variables each frame
        m_curSprite = Rect(0, 0, 32, 32);
        m_xVelocity = 0;
        m_yVelocity = SPEED;
    }
    else
    {
        m_xVelocity = 0;
    }
}

void Player::forceAirborne(float airtime, float velocity)
{
    if (!m_isAirborne)
        m_y-=10;
    m_isAirborne = true;
    m_yVelocity = velocity;
    m_airborneTimer = airtime;
}
// Adjusting the player's coordinates
void Player::movePlayer(float deltaTime)
{
    if (!m_collision)
    {
        m_x += m_xVelocity * deltaTime/1000.f;
        m_y += m_yVelocity * deltaTime/1000.f;
    }
    //if (y > 480)
    //y = -20;
}

// Respawn the player if they crashed
void Player::crash(float &crashTime, float deltaTime)
{
    if (crashTime > 2.0 && m_collision)
    {
        crashTime = 0;
        m_y = m_collidedObst.yPos + m_collidedObst.height;
        m_livesRemaining--;
        m_scoreMultiplier = 1;
        m_pointsToNextMultiplier = 0;
        if (m_livesRemaining==0)
        {
            m_dead = true;
        }
        m_collision = false;
    }
    else if (m_collision)
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
    if (m_scoreTimer > 0.1 && !m_collision)
    {
        m_score += m_yVelocity/10*m_scoreMultiplier; // the score is the players yVelocity times the multiplier
        m_scoreTimer = 0; // Reset the timer
    }
    else if (!m_collision)
    {
        m_scoreTimer += deltaTime/1000; // Adding to the timer
    }

    // Adjusting the multipler
    if (m_pointsToNextMultiplier > 0)
    {
        m_pointsToNextMultiplier -= 5 *m_scoreMultiplier/2* deltaTime/1000; // Subtracting the points to next multiplier
        if (m_pointsToNextMultiplier < 0 && m_scoreMultiplier >1) // If there's less than 0 points, subtract 1 from the multiplier
        {
            m_scoreMultiplier--;
            m_pointsToNextMultiplier = 100;
        }
    }
    if (m_timeSinceLastMultiply > 0)
    {
        m_timeSinceLastMultiply -= deltaTime/1000;
    }
}

// Adjusting the players multiplier
void Player::adjustMultiplier(float multiple)
{
    // adding to the points to next multiplier
    if (m_timeSinceLastMultiply <= 0)
        m_pointsToNextMultiplier += multiple;

    // If the points are more than 100, add to the multiplier
    if (m_pointsToNextMultiplier > 100 && m_scoreMultiplier<4)
    {
        m_scoreMultiplier++;
        m_pointsToNextMultiplier -= 100;
    }
    else if (m_scoreMultiplier == 4 && m_pointsToNextMultiplier > 150)
        m_pointsToNextMultiplier = 150;
    m_timeSinceLastMultiply = jumpTime;
}

// Drawing the player to the screen
void Player::draw(SDL_Rect camera, SDL_Renderer* renderer, SDL_Texture *spriteSheet)
{

    int playerHeight = 32;
    if (m_isAirborne)
        playerHeight = 64;

    SDL_Rect drawRect = {(int)m_x - camera.x, (int)m_y-camera.y, 32, playerHeight};

    if (m_collision)
    {
        m_isAirborne = false;
        m_curSprite = Rect(192, 0, 32, 32);
        drawRect.y += 16;
    }
    SDL_RenderCopy(renderer, spriteSheet, &m_curSprite, &drawRect);
}

void Player::renderLives(SDL_Renderer* renderer, SDL_Texture *lives)
{
    for (int x = 0, i = 0; x < m_livesRemaining; x++, i+=32)
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
    ss << m_score;
    string scoreOutput;
    ss >> scoreOutput;

    scoreOutput = "SCORE: " + scoreOutput;

    // Converting multipler to string
    stringstream ss1;
    ss1 << m_scoreMultiplier;
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