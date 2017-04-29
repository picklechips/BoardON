#ifndef BIOME_H
#define BIOME_H

#include <vector>

#include "obstacle.h"

using namespace std;

class Biome
{
public:
    Biome() {};
    Biome(vector<Obstacle> obsts, vector<int> amntObsts);

    void load(int bottomOfBiome);

     // Draw the biome to the background (after player passes)
    void spawnToBackground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);
     // Draw the biome to the foreground (before the player passes)
    void spawnToForeground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);
    
    void setSeed(double newSeed){seed = newSeed;}

private:
    vector<Obstacle> obstsInBiome; 
    vector<int> amountOfObst; 
    double seed;

};

#endif