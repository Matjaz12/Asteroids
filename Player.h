#pragma once

#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include "Asteroid.h"
#include <deque>

#define INTIAL_PLAYER_HEALTH 3
#define NUMBER_OF_BULLETS 5

// Player State
enum playerState
{
	STATE_STATIONARY,
	STATE_MOVING,
	STATE_SHOOTING,
	NUMBER_OF_PLAYER_STATES
};

class Player : public sf::Sprite
{

public:
	Player(const sf::Texture& _playerTexture, const sf::Texture& _bulletTexture);
	~Player();
private:

	// General
	int health, score;
	const float rotationSpeed = 0.25f;
	const sf::Texture playerTexture;
	const sf::Texture bulletTexture;
	// Shooting
	sf::Clock shotTimer;
	const float shootDelay = 100.f;
	bool isFirstBullet = true;
	const int numberOfBullets = NUMBER_OF_BULLETS;

	// Rocket thrust
	sf::Clock thrustTimer;
	const float speedBostTimeRange = 800.f;
	bool firstSpeedBost = true;
	const int boostFactor = 3;

public:

	playerState currentState = STATE_STATIONARY; // intial state
	std::deque<Bullet*> readyBullets;
	std::deque<Bullet*> movingBullets;

public:

	// Shooting
	void shootBullet();
	void reviveBullet(const int index);

	// Movement
	void movePlayer(const float dt);
	void rotatePlayer(const float dt , int direction);
	void moveBullets(const float dt);
	void startRocketThrustTimer();
	void slowDown();

	// Collision
	void playerCollides(Asteroid* asteroid);
	void bulletCollides(Asteroid* asteroid);

	// G&S
	int getHealth();
	int getScore();
	void resetParameters();

private:
	void setUp();
	void createBullets();
};

