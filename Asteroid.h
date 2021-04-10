#pragma once
#include <SFML/Graphics.hpp>
class Asteroid : public sf::Sprite
{
public:
	Asteroid(const sf::Texture& _asteroidTextureLevel0, const sf::Texture& _asteroidTextureLevel1 , const int _level = 1);
	~Asteroid();
private:
	int level;
	float speed = 1;
	const sf::Texture asteroidTextureLevel0;
	const sf::Texture asteroidTextureLevel1;
	sf::Vector2f direction;

public:
	void moveAsteroid(const float speed);
	void intializeAsteroid();
	void downSize();
	int  getLevel();
};

