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

    int xPos() {return (int)x;};
    int yPos() {return (int)y;};
    int yVel(){return yVelocity;}

    bool collided(){return collision;}; // Check if player collided
    void setCollision(bool hasCollided) {collision = hasCollided;};
    void obstCollidedWith(int pos, int height);

    void forceAirborne(float airtime, float velocity);
    bool IsInAir(){return isAirborne;} // Checking if the player is in the air
    
    void input(float deltaTime, float &jumpCooldown); // Taking player input
    void movePlayer(float deltaTime); // Moving the player
    
    void adjustMultiplier(float multiple);
    void adjustScore(float deltaTime);
    int getScore() {return score;}
    void renderScore(SDL_Renderer* renderer, TTF_Font* font);
    
    void crash(float &crashTime, float deltaTime); // Dealing with the player after they crashed
    void renderLives(SDL_Renderer* renderer, SDL_Texture *lives);
    bool isDead() {return dead;}
    void reset();

    void draw(SDL_Rect camera, SDL_Renderer* renderer, SDL_Texture *spriteSheet); // Drawing the player to the screen

private:
    float x, y;
    int yVelocity, xVelocity;

    SDL_Rect curSprite;

    bool isAirborne;
    float airborneTimer = 0;

    bool collision;
    int livesRemaining;

    ObstacleProperties collidedObst;
    bool dead;
    
    // KEEPING TRACK OF SCORE
    int score;
    int scoreMultiplier;
    float pointsToNextMultiplier;
    float scoreTimer; // Used to check when to add to the players score
    float timeSinceLastMultiply;
};

#endif