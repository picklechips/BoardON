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
            m_obstsInBiome.push_back(obsts[x]);
        }
    }
}

void Biome::SetSeed(double seed)
{
    m_seed = seed;
}

// Loading the Biome
void Biome::Load(int topOfBiome)
{
    srand(m_seed);
    vector <SDL_Rect> obstPositions;
    for (int x = 0; x < m_obstsInBiome.size(); x++)
    {
        int xLoc = rand() % 1304 - 64; // Randomizing the x location
        int yLoc = rand() % (960 - m_obstsInBiome[x].Height()) + topOfBiome; // Randomizing the y location
        if (x==0)
        {
            xLoc = -1000;
            yLoc = -1000;
        }
        if (yLoc + m_obstsInBiome[x].Height() > 960 + topOfBiome)
        {
            cout << "The obstacle is outside of the chunk!" << endl;
            yLoc -= m_obstsInBiome[x].Height() - (yLoc + m_obstsInBiome[x].Height() - 960 + topOfBiome);
        }
        // Calculating the sides of the obstacle trying to spawn
        int top = yLoc;
        int bottom = yLoc + m_obstsInBiome[x].Height();
        int right = xLoc + m_obstsInBiome[x].Width();
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
            m_obstsInBiome[x].SetPos(-1000, -1000);
        }
        else
        {
            obstPositions.push_back(Rect(xLoc, yLoc, m_obstsInBiome[x].Width(), m_obstsInBiome[x].Height()));
            m_obstsInBiome[x].SetPos(xLoc, yLoc);
        }
    }
}

// Spawning the biome to the foreground (Before the player passes the object)
void Biome::SpawnToForeground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer)
{
    for (int x = 0; x < m_obstsInBiome.size(); x++)
    {
        m_obstsInBiome[x].DrawToForeground(m_obstsInBiome[x].xPos(), m_obstsInBiome[x].yPos(), camera, deltaTime, player, renderer);
    }
}

// Spawning the biome to the background (After the player passes the object)
void Biome::SpawnToBackground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer)
{
    for (int x = 0; x < m_obstsInBiome.size(); x++)
    {
        m_obstsInBiome[x].DrawToBackground(m_obstsInBiome[x].xPos(), m_obstsInBiome[x].yPos(), camera, deltaTime, player, renderer);
    }
}