#ifndef BIOME_H
#define BIOME_H

#include <vector>

#include "obstacle.h"

class Biome
{
public:
    Biome() {};
    Biome(std::vector<Obstacle> obsts, std::vector<int> amntObsts);

    void load(int bottomOfBiome);

     // Draw the biome to the background (after player passes)
    void spawnToBackground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);
     // Draw the biome to the foreground (before the player passes)
    void spawnToForeground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);
    
    void setSeed(double newSeed){seed = newSeed;}

private:
    std::vector<Obstacle> obstsInBiome; 
    std::vector<int> amountOfObst; 
    double seed;

};

#endif