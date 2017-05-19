#include "obstacle.h"
#include "game.h"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

Obstacle::Obstacle()
{
    SDL_Rect tempRect = {32, 32, 32, 32};
    imgRect = tempRect;
    canCollide = false;
    isBgOnly = false;
    jumpedOver = false;
}
// Initializing obstacle WITHOUT collision
Obstacle::Obstacle(std::string name, int x, int y, int width, int height, bool bgOnly, SDL_Texture* obstacles)
{
    SDL_Rect tempRect = {x, y, width, height};
    imgRect = tempRect;
    canCollide = false;
    isBgOnly = bgOnly;
    jumpedOver = false;
    obstaclesSheet = obstacles;
}

// Initializing an obstacle WITH collision
Obstacle::Obstacle(std::string obstname, int x, int y, int width, int height, int collisionx, int collisiony, 
                    int collisionWidth, int collisionHeight, bool isJumpable, bool isbg, float ptstogive, SDL_Texture* obstacles)
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
    obstaclesSheet = obstacles;
}

void Obstacle::drawObstacle(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer)
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
        SDL_RenderCopy(renderer, obstaclesSheet, &imgRect, &drawRect);

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
        player.obstCollidedWith(getyPos(), getHeight());
        player.setCollision(true);
    }
    else if ((!player.collided() && (playerBottom-20 < obstBottom) && canCollide && playerRightSide > obstLeftSide && playerLeftSide < obstRightSide && playerTop < obstBottom && playerBottom > obstTop && ((jumpable && player.IsInAir()))))
    {
        player.adjustMultiplier(pointsToGive);

    }
    // Make sure if the player is trying to respawn he doesn't respawn on another object
    else if (player.collided() && canCollide && playerRightSide > obstLeftSide && playerLeftSide < obstRightSide && playerYRespawn < obstBottom && playerYRespawn+32 > obstTop)
    {
        player.obstCollidedWith(getyPos(), getHeight());
    }
}

void Obstacle::drawToBackground(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer)
{
    // Only draw if the player has passed the obstacle
    if (player.yPos()+22 >= y + imgRect.h || ((jumpable && player.IsInAir()) && player.yPos()+22 < y +imgRect.h) || isBgOnly)
    {
        drawObstacle(x, y, camera, deltaTime, player, renderer);
    }
}

void Obstacle::drawToForeground(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer)
{
    // Only draw if the player hasn't passed the obstacle
    if (player.yPos()+22 < y + imgRect.h && (!jumpable || (jumpable && !player.IsInAir())) && !isBgOnly)
    {
        drawObstacle(x, y, camera, deltaTime, player, renderer);
    }
}