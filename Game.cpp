#include "Game.h"
#include <iostream>


Game::Game(const int _width, const int _height )
	: width{_width}, height{_height}
{
	window = new sf::RenderWindow(sf::VideoMode(_width, _height), "title" , sf::Style::Titlebar | sf::Style::Close);
	currentAppState = STATE_MENU;

	loadAudio();
	setupMainWindow();
}

Game::~Game()
{
	delete player;
	delete window;
	for (auto* asteroid : asteroids)
	{
		delete asteroid;
	}
	asteroids.clear();
}

// Rendering
void Game::run()
{
	sf::Clock clock;
	window->setFramerateLimit(FPS);

	while (window->isOpen())
	{
		float dt = clock.restart().asMilliseconds(); // restarts the timer and returns the time since last restart
		
		if(currentAppState == STATE_GAME)
			isGameOver();
		checkForCollision();
		render(dt);
	
	}
}

void Game::render(const float dt)
{

	handleInput(dt);

	window->clear();

	switch (currentAppState)
	{
	case STATE_MENU:
		drawMenuWindow();
		break;
	case STATE_GAME:
		drawGameWindow(dt);
		break;
	case STATE_SCORELIST:
		drawScoreWindow();
		break;
	}

	window->display();
}

void Game::handleInput(const float dt)
{
	while (window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed) window->close();
		
		switch (currentAppState)
		{
		case STATE_MENU:

			if (event.type == event.KeyReleased)
			{

				keyPressedSound.play(); 

				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					if (selected - 1 >= 1)
					{
						menuComponents[selected].setFillColor(sf::Color::White);
						selected--;
						menuComponents[selected].setFillColor(sf::Color::Red);
					}
					break;
				case sf::Keyboard::Down:
					if (selected + 1 < NUM_OF_MAIN_WINDOW_COMPONENTS)
					{
						menuComponents[selected].setFillColor(sf::Color::White);
						selected++;
						menuComponents[selected].setFillColor(sf::Color::Red);
					}
					break;
				case sf::Keyboard::Enter:
					switchAppState();
					break;
				case sf::Keyboard::Space:
					switchAppState();
					break;
				}
			}
			break;
		case STATE_SCORELIST:
			if (event.type == event.KeyReleased)
			{
				keyPressedSound.play();

				switch (event.key.code)
				{
					case sf::Keyboard::Enter:
						currentAppState = STATE_MENU; // probi integrairat v switchAppstate ->selected = 0
						break;
					case sf::Keyboard::Space:
						currentAppState = STATE_MENU;
						break;
				}
			}
		}
	}// event Loop()

	if (currentAppState == STATE_GAME)
	{

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			player->rotatePlayer(dt, -1);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			player->rotatePlayer(dt, +1);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			if (player->currentState == STATE_STATIONARY)
			{
				player->startSpeedBoost();
				accelerationSound.play();
				std::cout << "Booooost...\n";
			}
			player->movePlayer(dt);
		}
		else
			player->slowDown();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !player->readyBullets.empty())
		{
			player->shootBullet();
			shotSound.play();
		}
	}
}

void Game::switchAppState()
{
	switch (selected)
	{
	case 0:
		currentAppState = STATE_MENU;
	case 1:
		setupGame();
		currentAppState = STATE_GAME;
		break;
	case 2:
		setupScoreWindow();
		currentAppState = STATE_SCORELIST;
		break;
	case 3:
		window->close();
		break;
	default:
		break;
	}
}

// Setup
void Game::setupMainWindow() 
{
	if (!font.loadFromFile("assets//gameFont.ttf"))
	{
		// handle error
		std::cout << "Error loading font!\n";
	}


	for (int i{ 0 }; i < NUM_OF_MAIN_WINDOW_COMPONENTS; i++)
	{
		menuComponents[i].setFont(font);
		switch (i)
		{
		case 0:
			menuComponents[i].setString("Asteroids");
			break;
		case 1:
			menuComponents[i].setString("Play");
			menuComponents[i].setFillColor(sf::Color::Red);
			break;
		case 2:
			menuComponents[i].setString("Top Score");
			break;
		case 3:
			menuComponents[i].setString("Exit");
			break;
		}
		menuComponents[i].setPosition(sf::Vector2f(width / 2 - menuComponents[i].getLocalBounds().width / 2, height / (NUM_OF_MAIN_WINDOW_COMPONENTS + 1) * (i +1)));
	}

}

void Game::setupScoreWindow()
{

	getScoreList();


	for (int i{ 0 }; i < NUM_OF_SCORE_WINDOW_COMPONENTS; i++)
	{
		scoreComponents[i].setFont(font);
		if(i==0) 
			scoreComponents[i].setString("Top Scores:");
		else if (i > 0 && i < NUM_OF_SCORE_COMPONENTS + 1)
		{
			if (i == 1) 
				scoreComponents[i].setFillColor(sf::Color::Green);
			if( i - 1 >= scoreArray.size()) // if scoreArray.size() NUM_OF_SCORE_COMPONENTS , aka less than 3 scores saved up to now 
				scoreComponents[i].setString("");
			else
				scoreComponents[i].setString(scoreArray[i - 1]);
		}
		else
		{
			scoreComponents[i].setString("Menu");
			scoreComponents[i].setFillColor(sf::Color::Red);
		}
		scoreComponents[i].setPosition(sf::Vector2f(width / 2 - scoreComponents[0].getLocalBounds().width / 2, height / (NUM_OF_SCORE_WINDOW_COMPONENTS + 1) * (i + 1)));
	}
}

void Game::setupGame()
{
	
	if (!gameStarted)
		loadTexture();


	player = new Player(playerTexture, bulletTexture);

	for (int i{ 0 }; i < NUM_OF_ASTEROIDS; i++)
		asteroids.push_back(new Asteroid(asteroidTextureLevel0 , asteroidTextureLevel1));
	
	// Setup game window components

	for (int i{ 0 }; i < NUM_OF_GAME_WINDOW_COMPONENTS; i++)
	{
		gameComponents[i].setFont(font);
		if (i == 0)
		{
			gameComponents[i].setString("Lives: " + std::to_string(player->getHealth()));
			gameComponents[i].setPosition(sf::Vector2f(0, 0));
		}
		else
		{
			gameComponents[i].setString("Score: " + std::to_string(player->getScore()));
			gameComponents[i].setPosition(sf::Vector2f(800 - gameComponents[i].getLocalBounds().width - 80, 0));
		}
	}

	gameStarted = true;

}

// Draw
void Game::drawMenuWindow()
{
	for (int i{ 0 }; i < NUM_OF_MAIN_WINDOW_COMPONENTS; i++)
	{
		window->draw(menuComponents[i]);
	}
}

void Game::drawScoreWindow()
{
	for (auto& scoreComponent : scoreComponents)
	{
		window->draw(scoreComponent);
	}
}

void Game::drawGameWindow(const float dt)
{

	for (int i{ 0 }; i < NUM_OF_GAME_WINDOW_COMPONENTS; i++)
	{
		if (i == 0)
			gameComponents[i].setString("Lives: " + std::to_string(player->getHealth()));

		if (i == 1)
			gameComponents[i].setString("Score: " + std::to_string(player->getScore()));

		window->draw(gameComponents[i]);
	}

	if (!player->movingBullets.empty())
	{
		player->moveBullets(dt);
		for(auto* bullet : player->movingBullets)
			window->draw(*bullet);
	}

	window->draw(*player);
	for (auto* asteroid : asteroids)
	{
		asteroid->moveAsteroid(dt);
		window->draw(*asteroid);
	}
}

// Game Logic

void Game::checkForCollision()
{
	int index = 0;
	int deadAsteroidIndex = -1;
	for (auto* asteroid : asteroids)
	{
		player->playerCollides(asteroid); // checks for player asteroid collision
		int levelBeforeCollisionCheck = asteroid->getLevel();
		if (!player->movingBullets.empty())
			player->bulletCollides(asteroid); // checks for bullet asteroid collsion
		if (asteroid->getLevel() != levelBeforeCollisionCheck)
		{
			deadAsteroidIndex = index;
			largeExplosion.play();
		}

		index++;
	}
	if (deadAsteroidIndex != -1) 
	{

		asteroids.push_back(new Asteroid(asteroidTextureLevel0,asteroidTextureLevel1,0));
		asteroids.back()->setPosition(asteroids[deadAsteroidIndex]->getPosition());
		deadAsteroidIndex = -1;
	}
}

void Game::isGameOver()
{
	if (player->getHealth() <= 0)
	{
		bool newBest = false;
		if (scoreArray.empty())
			getScoreList();

		if (scoreArray.size() < NUM_OF_SCORE_COMPONENTS)
		{
			scoreArray.push_back(std::to_string(player->getScore()));
			newBest = true;
		}
		else if (player->getScore() > std::stoi(scoreArray[NUM_OF_SCORE_COMPONENTS - 1]))
		{
			scoreArray[NUM_OF_SCORE_COMPONENTS - 1] = std::to_string(player->getScore());
			newBest = true;
		}
		if (newBest)
		{
			std::sort(scoreArray.begin(), scoreArray.end(), [](const std::string& strNum1, const std::string& strNum2)
				{
					if (std::stoi(strNum1) > std::stoi(strNum2))
						return true;
					return false;
				});
			saveScore();
		}

		// reset game and go to the main menu
		resetGame();
		currentAppState = STATE_MENU;
	}
}

void Game::resetGame()
{
	delete player;
	for (auto* asteroid : asteroids)
	{
		delete asteroid;
	}
	asteroids.clear();
}

// File I/0

void Game::getScoreList()
{
	std::ifstream fileIn{ "assets\\score.txt" };
	if (!fileIn)
	{
		std::cerr << "Error opening score.txt\n";
	}
	while (fileIn)
	{
		std::string strInput;
		fileIn >> strInput;
		if (strInput != "")
			scoreArray.push_back(strInput);
	}

	fileIn.close();
}

void Game::saveScore()
{
	std::ofstream fileOut{ "assets\\score.txt" };
	if (!fileOut)
	{
		std::cerr << "Error opening score.txt\n";
	}
	for (int i{0} ; i < scoreArray.size() ; i++)
		fileOut << scoreArray[i] << "\n";

	fileOut.close();
}

void Game::loadAudio()
{

	if (!buffer1.loadFromFile("assets//selectAudio.wav"))
	{
		std::cout << "Error loading audio file\n";
	}
	keyPressedSound.setBuffer(buffer1);

	if (!buffer2.loadFromFile("assets//shotAudio.wav"))
	{
		std::cout << "Error loading audio file\n";
	}
	shotSound.setBuffer(buffer2);

	if (!buffer3.loadFromFile("assets//accelerationAudio.wav"))
	{
		std::cout << "Error loading audio file\n";
	}
	accelerationSound.setBuffer(buffer3);

	if (!buffer4.loadFromFile("assets//bangLarge.wav"))
	{
		std::cout << "Error loading audio file\n";
	}
	largeExplosion.setBuffer(buffer4);

}

void Game::loadTexture()
{
	if (!playerTexture.loadFromFile("assets//triangle.png"))
	{
		// handle error
		std::cout << "Failed loading asteroid texture!\n";

	}
	if (!bulletTexture.loadFromFile("assets//bullet.png"))
	{
		// handle error
		std::cout << "Failed loading bullet texture!\n";

	}

	if (!asteroidTextureLevel0.loadFromFile("assets//asteroid_level0.png"))
	{
		// handle error
		std::cout << "Failed loading asteroid texture!\n";
	}

	if (!asteroidTextureLevel1.loadFromFile("assets//asteroid_level1.png"))
	{
		// handle error
		std::cout << "Failed loading asteroid texture!\n";
	}
}