#include "Bullet.h"

Bullet::Bullet(const sf::Texture& _texture)
:
texture{ _texture }
{
	setTexture(texture);
	scale(sf::Vector2f(0.15f, 0.15f));
}

void Bullet::intialize(const float _angle, const sf::Vector2f& startPosition)
{
	/*
	Function saves the angle at which the bullet was shot and its startingPosition
	*/
	angle = _angle;
	rotate(angle);
	setPosition(startPosition);
}

