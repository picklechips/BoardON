#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct ObstacleProperties {
    int yPos;
    int height;
} ObstacleProperties;

class Player
{
public:

    Player();

    const int SPEED = 200;
    const int HEIGHT = 32;
    const int WIDTH = 32;

    int xPos();
    int yPos();
    int yVel();

    bool collided(); // Check if player collided
    void setCollision(bool hasCollided);
    void obstCollidedWith(int pos, int height);

    void forceAirborne(float airtime, float velocity);
    bool IsInAir(); // Checking if the player is in the air
    
    void input(float deltaTime, float &jumpCooldown); // Taking player input
    void movePlayer(float deltaTime); // Moving the player
    
    void adjustMultiplier(float multiple);
    void adjustScore(float deltaTime);
    int getScore();
    void renderScore(SDL_Renderer* renderer, TTF_Font* font);
    
    void crash(float &crashTime, float deltaTime); // Dealing with the player after they crashed
    void renderLives(SDL_Renderer* renderer, SDL_Texture *lives);
    bool isDead();
    void reset();

    void draw(SDL_Rect camera, SDL_Renderer* renderer, SDL_Texture *spriteSheet); // Drawing the player to the screen

private:
    float m_x, m_y;
    int m_yVelocity, m_xVelocity;

    SDL_Rect m_curSprite;

    bool m_isAirborne;
    float m_airborneTimer = 0;

    bool m_collision;
    int m_livesRemaining;

    ObstacleProperties m_collidedObst;
    bool m_dead;
    
    // KEEPING TRACK OF SCORE
    int m_score;
    int m_scoreMultiplier;
    float m_pointsToNextMultiplier;
    float m_scoreTimer; // Used to check when to add to the players score
    float m_timeSinceLastMultiply;
};

#endif