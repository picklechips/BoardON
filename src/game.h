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
	static SDL_Window* Window();
	static SDL_Renderer* Renderer();

	// Sprite sheets
	static SDL_Texture* PlayerSpriteSheet();
	static SDL_Texture* ObstacleSpriteSheet();
	static SDL_Texture* LivesIMG();

	// Fonts
	static TTF_Font* DefaultFont();
	static TTF_Font* LargeFont();

	static Player& MainPlayer();

	// Game functions
	static bool Init();
	static void Close();
	
	static SDL_Texture* LoadTexture(std::string path);
	static bool LoadMedia();
	
	static void DrawTrails(SDL_Texture * src, SDL_Texture * curdst, SDL_Texture * prevdst, int yPos, SDL_Rect &camera);
	static void Draw(Biome curBiome, SDL_Rect camera, int biomeTop, Biome prevBiome, float deltaTime);
	static void RenderText(int x, int y, std::string text, SDL_Color color, TTF_Font * fnt);
	
	static Biome RandBiome(Biome biomes[], int amountOfBiomes);
	
	static void GameOver(bool& endgame);

private:
	static SDL_Window* m_window;
	static SDL_Renderer* m_renderer;


	static SDL_Texture* m_playerSpriteSheet;
	static SDL_Texture* m_obstacleSpriteSheet;
	static SDL_Texture* m_livesImg;

	static TTF_Font* m_defaultFont;
	static TTF_Font* m_largeFont;

	static Player m_player;
};

#endif