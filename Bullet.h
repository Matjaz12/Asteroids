#pragma once
#include <SFML/Graphics.hpp>

class Bullet : public sf::Sprite
{
private:
	sf::Texture texture;
public:
	Bullet(const sf::Texture& _texture);
	void intialize(const float _angle, const sf::Vector2f& startPosition);
	float angle{};
};

