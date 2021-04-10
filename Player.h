#pragma once
#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include "Asteroid.h"
#include <iostream>
#include <deque>

#define INTIAL_PLAYER_HEALTH 3
#define NUMBER_OF_BULLETS 5

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

	int health;
	int score;
	const sf::Texture playerTexture;
	const sf::Texture bulletTexture;


	sf::Clock shootTimer;
	const float shootDelay = 400.f;
	bool isFirstBullet = true;
	const int numberOfBullets = NUMBER_OF_BULLETS;

	sf::Clock speedBoostTimer;
	const float speedBostTimeRange = 600.f;
	bool firstSpeedBost = true;
	const int boostFactor = 3;

	const float rotationSpeed = 0.25f;

public:
	std::deque<Bullet*> readyBullets;
	std::deque<Bullet*> movingBullets;
	playerState currentState = STATE_STATIONARY; // intial player state

public:

	// Shooting
	void shootBullet();
	void reviveBullet(const int index);

	// Movement
	void movePlayer(const float dt);
	void rotatePlayer(const float dt , int direction);
	void moveBullets(const float dt);
	void startSpeedBoost();
	void slowDown();
	// Collision
	void playerCollides(Asteroid* asteroid);
	void bulletCollides(Asteroid* asteroid);

	// Getters&Setters
	int getHealth();
	int getScore();
	void resetParameters();
private:
	void createBullets();
};

