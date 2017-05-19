#ifndef GAME_H
#define GAME_H

#include "biome.h"

const int SCREEN_HEIGHT = 640;
const int SCREEN_WIDTH = 900;

class Game
{
public:
	Game();
	~Game();
	
	// Main window
	static SDL_Window* Window;
	static SDL_Renderer* Renderer;

	// Sprite sheets
	static SDL_Texture* PlayerSpriteSheet;
	static SDL_Texture* Obstacles;
	static SDL_Texture* lives;

	// Fonts
	static TTF_Font* font;
	static TTF_Font* largeFont;

	static Player player;

	// Game functions
	static bool init();
	static void close();
	
	static SDL_Texture* loadTexture(std::string path);
	static bool loadMedia();
	
	static void drawTrails(SDL_Texture * src, SDL_Texture * curdst, SDL_Texture * prevdst, int yPos, SDL_Rect &camera);
	static void draw(Biome curBiome, SDL_Rect camera, int biomeTop, Biome prevBiome, float deltaTime);
	static void RenderText(int x, int y, std::string text, SDL_Color color, TTF_Font * fnt);
	
	static Biome RandBiome(Biome biomes[], int amountOfBiomes);
	
	static void GameOver(bool& endgame);
};

#endif