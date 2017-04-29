#include <iostream>

#include "biome.h"
#include "obstacle.h"

using namespace std;

// Used to set a SDL_Rect without needing to create a temporary rect
SDL_Rect Rect(int x, int y, int width, int height);

Biome::Biome(vector<Obstacle> obsts, vector<int> amntObsts)
{
    // Storing the ostacles in a vector
    for (int x = 0; x < obsts.size(); x++)
    {
        for (int y = 0; y < amntObsts[x]; y++)
        {
            // Adding the amount and type of each obstacle to the vector
            obstsInBiome.push_back(obsts[x]);
        }
    }
}
Biome::~Biome()
{
    //delete[]obstsInBiome; // deleting the obstacle array
}

// Loading the Biome
void Biome::load(int topOfBiome)
{
    srand(seed);
    vector <SDL_Rect> obstPositions;
    for (int x = 0; x < obstsInBiome.size(); x++)
    {
        int xLoc = rand() % 1304 - 64; // Randomizing the x location
        int yLoc = rand() % (960 - obstsInBiome[x].getHeight()) + topOfBiome; // Randomizing the y location
        if (x==0)
        {
            xLoc = -1000;
            yLoc = -1000;
        }
        if (yLoc + obstsInBiome[x].getHeight() > 960 + topOfBiome)
        {
            cout << "The obstacle is outside of the chunk!" << endl;
            yLoc -= obstsInBiome[x].getHeight() - (yLoc + obstsInBiome[x].getHeight() - 960 + topOfBiome);
        }
        // Calculating the sides of the obstacle trying to spawn
        int top = yLoc;
        int bottom = yLoc + obstsInBiome[x].getHeight();
        int right = xLoc + obstsInBiome[x].getWidth();
        int left = xLoc;

        for (int i = 0; i < obstPositions.size(); i++)
        {
            // Calculating the sides of the already spawned object
            int spawnedTop = obstPositions[i].y;
            int spawnedBottom = obstPositions[i].y + obstPositions[i].h;
            int spawnedRight = obstPositions[i].x + obstPositions[i].w;
            int spawnedLeft = obstPositions[i].x;
            while (bottom > spawnedTop && right > spawnedLeft && left < spawnedRight && top < spawnedBottom)
            {
                yLoc += obstPositions[i].h - (top - spawnedTop - 10);
                bottom += obstPositions[i].h - (top - spawnedTop - 10);
                top += obstPositions[i].h - (top - spawnedTop - 10);
                i = 0;
            }
        }
        // If the object is pushed too far down, into the next chunk, spawn it off screen to avoid overlapping.
        if (bottom > topOfBiome+960)
        {
            obstsInBiome[x].setxy(-1000, -1000);
        }
        else
        {
            obstPositions.push_back(Rect(xLoc, yLoc, obstsInBiome[x].getWidth(), obstsInBiome[x].getHeight()));
            obstsInBiome[x].setxy(xLoc, yLoc);
        }
    }
}

// Spawning the biome to the foreground (Before the player passes the object)
void Biome::spawnToForeground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer)
{
    for (int x = 0; x < obstsInBiome.size(); x++)
    {
        obstsInBiome[x].drawToForeground(obstsInBiome[x].getxPos(), obstsInBiome[x].getyPos(), camera, deltaTime, player, renderer);
    }
}

// Spawning the biome to the background (After the player passes the object)
void Biome::spawnToBackground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer)
{
    for (int x = 0; x < obstsInBiome.size(); x++)
    {
        obstsInBiome[x].drawToBackground(obstsInBiome[x].getxPos(), obstsInBiome[x].getyPos(), camera, deltaTime, player, renderer);
    }
}