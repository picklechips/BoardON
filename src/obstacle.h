#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <string>

#include "player.h"

using namespace std;

class Obstacle
{
private:
    string name;

    int xPos;
    int yPos;
    
    bool canCollide;
    SDL_Rect collisionRect;
    bool isBgOnly;
    
    bool jumpable;
    bool jumpedOver;

    float pointsToGive;
    
    SDL_Texture* obstaclesSheet;
    SDL_Rect imgRect;

public:
    Obstacle();
    Obstacle(string name, int x, int y, int height, int width, bool isBgOnly, SDL_Texture* obstacles); // Obstacle without collision
    Obstacle(string name, int x, int y, int height, int width, int collisionx, int collisiony, int collisionWidth, int collisionHeight, 
                bool jumpable, bool isBgOnly, float ptstogive, SDL_Texture* obstacles); // Obstacle with collision

    void drawObstacle(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);
    

    // Since After the player passes an obstacle, he is in front of it, the obstacle should be drawn to the
    // background (behind the player). But if the player has not yet passed the object, it should be
    // drawn to the foreground (in front of the player). To avoid drawing an object twice each frame (once
    // to the background and once to the foreground) Two different functions are used, so an object is only
    // drawn to the screen once. drawToBackground is called before the player is drawn, and drawToForeground
    // is called AFTER the player is drawn.
    void drawToBackground(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);
    void drawToForeground(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);

    void setxy(int x, int y){xPos = x;yPos = y;}
    int getxPos(){return xPos;};
    int getyPos(){return yPos;};
    int getWidth(){return imgRect.w;};
    int getHeight(){return imgRect.h;};
    bool isJumpable(){return jumpable;}
};

#endif