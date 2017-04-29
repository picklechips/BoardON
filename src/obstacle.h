#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <string>

#include "player.h"

using namespace std;

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
    SDL_Texture* obstaclesSheet;

protected:
    int xPos, yPos;
    SDL_Rect imgRect;

public:
    Obstacle();
    Obstacle(string name, int x, int y, int height, int width, bool isBgOnly, SDL_Texture* obstacles); // Obstacle without collision
    Obstacle(string name, int x, int y, int height, int width, int collisionx, int collisiony, int collisionWidth, int collisionHeight, 
                bool jumpable, bool isBgOnly, float ptstogive, SDL_Texture* obstacles); // Obstacle with collision
    void drawObstacle(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer); // Drawing the obstacle to the screen
    virtual void drawToBackground(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer); // Drawing it to the screen after the player passed
    virtual void drawToForeground(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer); // Drawing it to the screen before the player has passed
    void setxy(int x, int y){xPos = x;yPos = y;}
    int getxPos(){return xPos;};
    int getyPos(){return yPos;};
    int getWidth(){return imgRect.w;};
    int getHeight(){return imgRect.h;};
    bool isJumpable(){return jumpable;}
};

#endif