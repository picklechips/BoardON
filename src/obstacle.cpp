#include "obstacle.h"
#include "game.h"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

Obstacle::Obstacle() :
    m_canCollide {false},
    m_isBgOnly {false}
{
    SDL_Rect tempRect = {32, 32, 32, 32};
    m_imgRect = tempRect;
}
// Initializing obstacle WITHOUT collision
Obstacle::Obstacle(std::string name, int x, int y, int width, int height, bool bgOnly, SDL_Texture* obstacles) :
    m_canCollide {false},
    m_isBgOnly {bgOnly},
    m_obstaclesSheet {obstacles}
{
    SDL_Rect tempRect = {x, y, width, height};
    m_imgRect = tempRect;
}

// Initializing an obstacle WITH collision
Obstacle::Obstacle(std::string obstname, int x, int y, int width, int height, int collisionx, int collisiony, 
                    int collisionWidth, int collisionHeight, bool isJumpable, bool isbg, float ptstogive, SDL_Texture* obstacles) :
    m_canCollide {true},
    m_jumpable {isJumpable},
    m_isBgOnly {isbg},
    m_name {obstname},
    m_pointsToGive {ptstogive},
    m_obstaclesSheet {obstacles}
{
    SDL_Rect colRect = {collisionx, collisiony, collisionWidth, collisionHeight};
    SDL_Rect tempRect = {x, y, width, height};
    m_imgRect = tempRect;
    m_collisionRect = colRect;
}

//Accessors
void Obstacle::SetPos(int x, int y)
{
    m_xPos = x;
    m_yPos = y;
}
int Obstacle::xPos()
{
    return m_xPos;
}
int Obstacle::yPos()
{
    return m_yPos;
}
int Obstacle::Width()
{
    return m_imgRect.w;
}
int Obstacle::Height()
{
    return m_imgRect.h;
}

void Obstacle::DrawObstacle(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer)
{
    m_xPos = x;
    m_yPos = y;

    // Setting the collision box around the obstacle
    SDL_Rect collideRect = m_collisionRect;
    collideRect.x = m_collisionRect.x + x + camera.x;
    collideRect.y = m_collisionRect.y + y;

    // Setting the rect to draw the image
    SDL_Rect drawRect = {x-camera.x, y-camera.y, m_imgRect.w, m_imgRect.h};

    // Drawing the image
    // Only draw it if it's present on the screen
    if (m_xPos + m_imgRect.w > camera.x && m_xPos < SCREEN_WIDTH+camera.x && m_yPos + m_imgRect.h > camera.y && m_yPos < SCREEN_HEIGHT + camera.y)
        SDL_RenderCopy(renderer, m_obstaclesSheet, &m_imgRect, &drawRect);

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
    int playerYRespawn =  y+m_imgRect.h;

    if (m_name == "cliff" && player.yPos()+32 > y+20 && playerLeftSide < obstRightSide-3 &&
        playerRightSide > obstLeftSide+3 && playerTop < y+114)
    {
       if (player.yVel()<= player.SPEED/3)
            player.forceAirborne(0.05, player.SPEED/2.5f);
        else
            player.forceAirborne(0.05, player.SPEED*1.5);
    }

    //Checking for collision (Checking if the player and the obstacle overlap)
    if (!player.collided() && (playerBottom-20 < obstBottom) && m_canCollide && playerRightSide > obstLeftSide && 
        playerLeftSide < obstRightSide && playerTop < obstBottom && playerBottom > obstTop && 
        ((m_jumpable && !player.IsInAir()) || !m_jumpable))
    {
        player.obstCollidedWith(yPos(), m_imgRect.h);
        player.setCollision(true);
    }
    else if ((!player.collided() && (playerBottom-20 < obstBottom) && m_canCollide && playerRightSide > obstLeftSide &&
            playerLeftSide < obstRightSide && playerTop < obstBottom && playerBottom > obstTop && ((m_jumpable && player.IsInAir()))))
    {
        player.adjustMultiplier(m_pointsToGive);

    }
    // Make sure if the player is trying to respawn he doesn't respawn on another object
    else if (player.collided() && m_canCollide && playerRightSide > obstLeftSide && playerLeftSide < obstRightSide &&
             playerYRespawn < obstBottom && playerYRespawn+32 > obstTop)
    {
        player.obstCollidedWith(yPos(), m_imgRect.w);
    }
}

void Obstacle::DrawToBackground(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer)
{
    // Only draw if the player has passed the obstacle
    if (player.yPos()+22 >= y + m_imgRect.h || ((m_jumpable && player.IsInAir()) && player.yPos()+22 < y + m_imgRect.h) || m_isBgOnly)
    {
        DrawObstacle(x, y, camera, deltaTime, player, renderer);
    }
}

void Obstacle::DrawToForeground(int x, int y, SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer)
{
    // Only draw if the player hasn't passed the obstacle
    if (player.yPos()+22 < y + m_imgRect.h && (!m_jumpable || (m_jumpable && !player.IsInAir())) && !m_isBgOnly)
    {
        DrawObstacle(x, y, camera, deltaTime, player, renderer);
    }
}