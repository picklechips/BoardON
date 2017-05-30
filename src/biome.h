#ifndef BIOME_H
#define BIOME_H

#include <vector>

#include "obstacle.h"

class Biome
{
public:
    Biome() {};
    Biome(std::vector<Obstacle> obsts, std::vector<int> amntObsts);

    void Load(int bottomOfBiome);

     // Draw the biome to the background (after player passes)
    void SpawnToBackground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);
     // Draw the biome to the foreground (before the player passes)
    void SpawnToForeground(SDL_Rect camera, float deltaTime, Player &player, SDL_Renderer* renderer);
    
    void SetSeed(double seed);

private:
    std::vector<Obstacle> m_obstsInBiome; 
    std::vector<int> m_amountOfObst; 
    double m_seed;

};

#endif