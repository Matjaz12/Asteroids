#include "Game.h"
#include <iostream>


Game::Game(const int _width, const int _height )
	: 
	width{_width}, height{_height},
	currentAppState{ STATE_MENU }
{
	// Create a Non Resizable window
	window = new sf::RenderWindow(sf::VideoMode(_width, _height), "Asteroids Game" , sf::Style::Titlebar | sf::Style::Close);
	window->setFramerateLimit(FPS);

	// Try to Load necessary Files
	loadTextures();
	loadAudio();
	setupMainWindow();
}

Game::~Game()
{
	delete window;
	deallocateMemory();
}

// Rendering

void Game::deallocateMemory()
{
	delete player;
	for (auto* asteroid : asteroids)
	{
		delete asteroid;
	}
	asteroids.clear();
}

void Game::run()
{
	/*
	Function contains the Main Game Loop()
	it takes care of Game Logic
	*/

	sf::Clock clock;
	while (window->isOpen())
	{
		float dt = clock.restart().asMilliseconds(); // restarts the timer and returns the time since last restart
		if(currentAppState == STATE_GAME) isGameOver();
		checkForCollision();
		render(dt); // pass dt, to render function to ensure Frame Indepented Game Play
	
	}
}

void Game::render(const float dt)
{
	/*
	Function calls the appropriate draw member function which 
	corresponds to the Current Application State
	*/
	handleUserInput(dt);

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

void Game::handleUserInput(const float dt)
{
	/*
	Function takes care of User Input, it modifies the state of the Game Object
	the meaning of specific key events changes based on the current Application State
	*/
	while (window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window->close();
		
		switch (currentAppState)
		{

		case STATE_MENU:

			if (event.type == event.KeyReleased)
			{
				keyPressedSound.play(); 
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					menuMoveUp(); 
					break;
				case sf::Keyboard::Down:
					menuMoveDown();
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
						currentAppState = STATE_MENU; 
						break;
					case sf::Keyboard::Space:
						currentAppState = STATE_MENU;
						break;
				}
			}
		default:
			break;
		}
	}// Event Loop()

	if (currentAppState == STATE_GAME)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			player->rotatePlayer(dt, -1); // rotatePlayer in negative direction
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			player->rotatePlayer(dt, +1); // rotatePlayer in positive direction

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			if (player->currentState == STATE_STATIONARY) // if player just pressed the Up Arrow Key
			{
				player->startRocketThrustTimer(); // start the thrust timer
				accelerationSound.play();
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
	}// currentAppState == STATE_GAME

}

void Game::menuMoveUp()
{
	/*
	Switches the currently selected Main Menu Component
	*/
	if (selected - 1 >= 1)
	{
		menuComponents[selected].setFillColor(sf::Color::White);
		selected--;
		menuComponents[selected].setFillColor(sf::Color::Red);
	}
}

void Game::menuMoveDown()
{
	/*
	Switches the currently selected Main Menu Component
	*/
	if (selected + 1 < NUM_OF_MAIN_WINDOW_COMPONENTS)
	{
		menuComponents[selected].setFillColor(sf::Color::White);
		selected++;
		menuComponents[selected].setFillColor(sf::Color::Red);
	}
}

void Game::switchAppState()
{
	/*
	Function switches the current Application State,
	and calls the necessary member functions before we draw the window
	*/
	switch (selected)
	{
	case 0:
		currentAppState = STATE_MENU;
	case 1:
		setupGame();
		currentAppState = STATE_GAME;
		break;
	case 2:
		getScoreList();
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
	/*
	Function creates and intilaizes Main Menu Components
	*/

	if (!font.loadFromFile("assets//gameFont.ttf"))
	{
		throw std::exception();
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
	/*
	Function creates and intilaizes Score Window Components
	*/

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
	/*
	Function creates all game objects: the player (that creates bullets) and the
	asteroids, it also creates the game window components
	*/
	player = new Player(playerTexture, bulletTexture);

	for (int i{ 0 }; i < NUM_OF_ASTEROIDS; i++)
		asteroids.push_back(new Asteroid(asteroidTextureLevel0 , asteroidTextureLevel1));
	
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
	/*
	Function draws all Main Menu Components
	*/
	for (auto& menuComponent : menuComponents)
	{
		window->draw(menuComponent);
	}
}

void Game::drawScoreWindow()
{
	/*
	Function draws all Main Menu Components
	*/

	for (auto& scoreComponent : scoreComponents)
	{
		window->draw(scoreComponent);
	}
}

void Game::drawGameWindow(const float dt)
{
	/*
	Function draws all Game Window components
	and all Game Objects
	*/
	for (int i{ 0 }; i < NUM_OF_GAME_WINDOW_COMPONENTS; i++)
	{
		if (i == 0)
			gameComponents[i].setString("Lives: " + std::to_string(player->getHealth()));

		if (i == 1)
			gameComponents[i].setString("Score: " + std::to_string(player->getScore()));

		window->draw(gameComponents[i]);
	}

	// draw player
	window->draw(*player);

	// draw Bullets 
	if ( !player->movingBullets.empty() ) // if there are any bullets flying around
	{
		player->moveBullets(dt);
		for(auto* bullet : player->movingBullets)
			window->draw(*bullet);
	}

	// draw Asteroids
	for (auto* asteroid : asteroids)
	{
		asteroid->moveAsteroid(dt);
		window->draw(*asteroid);
	}
}

// Game Logic

void Game::checkForCollision()
{
	/*
	Function checks for collision between Player and all Asteroids
	and collision between currently moving bullets and all Asteroids
	*/
	int index = 0;
	int deadAsteroidIndex = -1;

	for (auto* asteroid : asteroids)
	{
		player->playerCollides(asteroid);

		int levelBeforeCollisionCheck = asteroid->getLevel();
		if (!player->movingBullets.empty()) // if there are any bullets flying around
			player->bulletCollides(asteroid);
		if (asteroid->getLevel() != levelBeforeCollisionCheck) // if level of asteroid changed, collision accured, so we save the asteroid we want to destroy -> generate new ones
		{
			deadAsteroidIndex = index;
			largeExplosion.play();
		}
		index++;
	}
	if (deadAsteroidIndex != -1) // if asteroid was just destroyed
	{
		asteroids.push_back(new Asteroid(asteroidTextureLevel0,asteroidTextureLevel1,0)); // create another asteroid
		asteroids.back()->setPosition(asteroids[deadAsteroidIndex]->getPosition()); 
		deadAsteroidIndex = -1;
	}
}

void Game::isGameOver()
{
	/*
	Function checks if Game is Over, in case of new best score
	it saves the score in appropirate position and resets the game
	*/
	if (player->getHealth() <= 0)
	{

		if (scoreArray.empty()) // if user has not opened the score window yet
			getScoreList();

		bool newBest = false;
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
		// If Game Over Deallocate Memory and switch the game state
		deallocateMemory();
		currentAppState = STATE_MENU;
	}
}

// File I/0

void Game::getScoreList()
{
	/*
	Function opens the score.txt file and saves the list of current best scores to a vector
	*/
	std::ifstream fileIn{ "assets\\score.txt" };
	if (!fileIn)
	{
		std::cerr << "Error opening score.txt\n";
	}
	else
	{
		while (fileIn)
		{
			std::string strInput;
			fileIn >> strInput;
			if (strInput != "")
				scoreArray.push_back(strInput);
		}

		fileIn.close();
	}
}

void Game::saveScore()
{
	/*
	Function saves the new best score to the score.txt file
	*/
	std::ofstream fileOut{ "assets\\score.txt" };
	if (!fileOut)
	{
		std::cerr << "Error opening score.txt\n";
	}
	else
	{
		for (int i{ 0 }; i < scoreArray.size(); i++)
			fileOut << scoreArray[i] << "\n";

		fileOut.close();
	}
}

void Game::loadAudio()
{

	if (!buffer1.loadFromFile("assets//selectAudio.wav"))
	{
		throw std::exception();
	}
	keyPressedSound.setBuffer(buffer1);

	if (!buffer2.loadFromFile("assets//shotAudio.wav"))
	{
		throw std::exception();
	}
	shotSound.setBuffer(buffer2);

	if (!buffer3.loadFromFile("assets//accelerationAudio.wav"))
	{
		throw std::exception();
	}
	accelerationSound.setBuffer(buffer3);

	if (!buffer4.loadFromFile("assets//bangLarge.wav"))
	{
		throw std::exception();
	}
	largeExplosion.setBuffer(buffer4);

}

void Game::loadTextures()
{
	if (!playerTexture.loadFromFile("assets//player.png"))
	{
		throw std::exception();
	}
	if (!bulletTexture.loadFromFile("assets//bullet.png"))
	{
		throw std::exception();
	}

	if (!asteroidTextureLevel0.loadFromFile("assets//asteroid_level0.png"))
	{
		throw std::exception();
	}

	if (!asteroidTextureLevel1.loadFromFile("assets//asteroid_level1.png"))
	{
		throw std::exception();
	}
}