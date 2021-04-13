#pragma once

#include "Player.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <string>
#include <array>
#include <algorithm>

#define FPS 60
#define NUM_OF_MAIN_WINDOW_COMPONENTS 4
#define NUM_OF_SCORE_COMPONENTS 3
#define NUM_OF_SCORE_WINDOW_COMPONENTS 5
#define NUM_OF_GAME_WINDOW_COMPONENTS 2
#define NUM_OF_ASTEROIDS 6

class Game
{
public:
	Game(const int _width, const int _height);
	~Game();
private:
	
	// Components
	int width; int height;
	sf::RenderWindow* window = nullptr;
	sf::Event event;
	sf::Font font;

	// Audio
	sf::SoundBuffer buffer1;
	sf::Sound keyPressedSound;
	sf::SoundBuffer buffer2;
	sf::Sound shotSound;
	sf::SoundBuffer buffer3;
	sf::Sound accelerationSound;
	sf::SoundBuffer buffer4;
	sf::Sound largeExplosion;

	// Application State

	enum appState
	{
		STATE_MENU,
		STATE_GAME,
		STATE_SCORELIST,
		NUMBER_OF_STATES
	};

	appState currentAppState;

	// Menu
	int selected = 1;
	sf::Text menuComponents[NUM_OF_MAIN_WINDOW_COMPONENTS];

	// Score
	sf::Text scoreComponents[NUM_OF_SCORE_WINDOW_COMPONENTS];
	std::vector<std::string> scoreArray;

	// Game
	bool gameStarted = false;
	Player* player = nullptr;
	std::vector<Asteroid*> asteroids;
	sf::Text gameComponents[NUM_OF_GAME_WINDOW_COMPONENTS];
	sf::Texture playerTexture;
	sf::Texture bulletTexture;
	sf::Texture asteroidTextureLevel0;
	sf::Texture asteroidTextureLevel1;

public:
	void run();
	void deallocateMemory();
private:
	// Render
	void render(const float dt);
	void handleUserInput(const float dt);
	void menuMoveUp();
	void menuMoveDown();
	void switchAppState();

private:
	// Draw
	void drawMenuWindow();
	void drawGameWindow(const float dt);
	void drawScoreWindow();

private:
	// Setup
	void setupGame();
	void setupMainWindow();
	void setupScoreWindow();

private:
	// Game Logic
	void checkForCollision();
	void isGameOver();

	// File I/O
	void getScoreList();
	void loadTextures();
	void saveScore();
	void loadAudio();
};

