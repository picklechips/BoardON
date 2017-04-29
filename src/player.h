#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

typedef struct ObstacleProperties {
    int yPos;
    int height;
} ObstacleProperties;

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
    ObstacleProperties collidedObst;
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
    void draw(SDL_Rect camera, SDL_Renderer* renderer, SDL_Texture *spriteSheet); // Drawing the player to the screen
    void movePlayer(float deltaTime); // Moving the player
    void crash(float &crashTime, float deltaTime); // Dealing with the player after they crashed
    void forceAirborne(float airtime, float velocity);
    void obstCollidedWith(int pos, int height){collidedObst.yPos = pos; collidedObst.height = height;} // setting the obstacle that the player collided with
    void adjustScore(float deltaTime);
    void adjustMultiplier(float multiple);
    void renderLives(SDL_Renderer* renderer, SDL_Texture *lives);
    void renderScore(SDL_Renderer* renderer, TTF_Font* font);
    bool isDead(){return dead;}
    int getScore(){return score;}
    void reset();
};

#endif