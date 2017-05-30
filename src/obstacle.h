#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <string>

#include "player.h"

class Obstacle
{
public:
    Obstacle();
    Obstacle(std::string name, int x, int y, int height, int width, bool isBgOnly, SDL_Texture* obstacles); // Obstacle without collision
    Obstacle(std::string name, int x, int y, int height, int width, int collisionx, int collisiony, int collisionWidth, int collisionHeight, 
                bool jumpable, bool isBgOnly, float ptstogive, SDL_Texture* obstacles); // Obstacle with collision

    void DrawObstacle(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);
    

    // Since After the player passes an obstacle, he is in front of it, the obstacle should be drawn to the
    // background (behind the player). But if the player has not yet passed the object, it should be
    // drawn to the foreground (in front of the player). To avoid drawing an object twice each frame (once
    // to the background and once to the foreground) Two different functions are used, so an object is only
    // drawn to the screen once. drawToBackground is called before the player is drawn, and drawToForeground
    // is called AFTER the player is drawn.
    void DrawToBackground(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);
    void DrawToForeground(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);

    void SetPos(int x, int y);
    int xPos();
    int yPos();
    int Width();
    int Height();

private:
    std::string m_name;

    int m_xPos;
    int m_yPos;
    
    bool m_canCollide;
    SDL_Rect m_collisionRect;
    bool m_isBgOnly;
    
    bool m_jumpable;

    float m_pointsToGive;
    
    SDL_Texture* m_obstaclesSheet;
    SDL_Rect m_imgRect;
};

#endif