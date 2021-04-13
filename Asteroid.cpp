#include "Asteroid.h"
#include <iostream>

Asteroid::Asteroid(const sf::Texture& _asteroidTextureLevel0 , const sf::Texture& _asteroidTextureLevel1 , const int _level)
	:
	asteroidTextureLevel0{ _asteroidTextureLevel0},
	asteroidTextureLevel1{ _asteroidTextureLevel1},
	level{_level},
	madeDamage{false}
{
	scale(sf::Vector2f(0.060f, 0.060f));
	setOrigin(getPosition().x + getLocalBounds().width / 2, getPosition().y + getLocalBounds().height / 2);
	intializeAsteroid();
}

Asteroid::~Asteroid()
{
}

int getRandomNumber(int min, int max)
{
	/*
	Retruns random number in range(min,max)
	*/
	static constexpr double fraction{ 1.0 / (RAND_MAX + 1.0) };  
	return min + static_cast<int>((max - min + 1) * (std::rand() * fraction));
}

void Asteroid::moveAsteroid(const float dt)
{
	/*
	Function moves the asteroid in its direction, and does some bound checking
	*/
	move(dt*speed*direction);

	if (getPosition().x > 1000)		 setPosition(sf::Vector2f(0, getPosition().y));
	else if (getPosition().x < -200) setPosition(sf::Vector2f(800, getPosition().y));
	else if (getPosition().y > 1000) setPosition(sf::Vector2f(getPosition().x, 0));
	else if (getPosition().y < -200) setPosition(sf::Vector2f(getPosition().x, 800));
}

void Asteroid::intializeAsteroid()
{
	/*
	Function intializes Asteroid based on its current level
	*/
	switch (level)
	{
	case 0:
		setTexture(asteroidTextureLevel0);
		direction = { static_cast <float> (rand()) / static_cast <float> (RAND_MAX) ,  static_cast <float> (rand()) / static_cast <float> (RAND_MAX) }; // range 0.0f - 1.0f
		speed = 0.35f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.40f - 0.35f)));
		break;

	case 1:
		setTexture(asteroidTextureLevel1);
		direction = { static_cast <float> (rand()) / static_cast <float> (RAND_MAX) ,  static_cast <float> (rand()) / static_cast <float> (RAND_MAX) }; // range 0.0f - 1.0f
		speed = 0.15f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.25f - 0.15f)));
		setPosition(
			0.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (800.0f - 0.0f))),
			0.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (800.0f - 0.0f)))
		);
		if (direction.x < 0.02f && direction.y < 0.1f)
			speed = 10 * speed;
		break;
	default:
		break;
	}
}

void Asteroid::downSize()
{
	/*
	Function calls intializeAsteroid with appropirate level
	*/
	if (level == 1)
	{
		level -= 1;
		intializeAsteroid();
	}
	else if (level == 0)
	{
		level += 1;
		intializeAsteroid();
	}

}

int Asteroid::getLevel()
{
	return level;
}