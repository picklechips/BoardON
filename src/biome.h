#ifndef BIOME_H
#define BIOME_H

#include <vector>

#include "obstacle.h"

using namespace std;

// Biome class, allowing for varying environments
class Biome
{
private:
    vector<Obstacle> obstsInBiome; // Store the potential obstacles in the biome
    vector<int> amountOfObst; // The max amount of an obstacle, works respectively with obstsInBiome
    double seed;
public:
    Biome() {};
    Biome(vector<Obstacle> obsts, vector<int> amntObsts);
    ~Biome();
    void load(int bottomOfBiome);
    void spawnToBackground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer); // Draw the biome to the background (after player passes)
    void spawnToForeground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer); // Draw the biome to the foreground (before the player passes)
    void setSeed(double newSeed)
    {seed = newSeed;} // Setting the seed
};

#endif