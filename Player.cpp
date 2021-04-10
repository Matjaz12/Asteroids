#include "Player.h"

Player::Player(const sf::Texture& _playerTexture, const sf::Texture& _bulletTexture)
	:
	playerTexture{ _playerTexture },
	bulletTexture{_bulletTexture },
	health{ INTIAL_PLAYER_HEALTH},
	score{ 0 }

{
	// Setup sprite

	setTexture(playerTexture);
	scale(sf::Vector2f(0.015f, 0.025f));
	setOrigin(400.0f + getLocalBounds().width / 2, 400.f + getLocalBounds().height / 2);
	setPosition(400.0f, 400.0f);
	createBullets();
}

Player::~Player()
{
	
	while (readyBullets.size() != 0)
	{
		Bullet* currentBullet = readyBullets.back();
		readyBullets.pop_back();
		delete currentBullet;
	}

	while (movingBullets.size() != 0)
	{
		Bullet* currentBullet = movingBullets.back();
		movingBullets.pop_back();
		delete currentBullet;
	}

}

// Shooting
void Player::createBullets()
{
	for (int i{ 0 }; i < numberOfBullets; i++)
		readyBullets.push_back(new Bullet(bulletTexture));
}

void Player::shootBullet()
{
	if (shootTimer.getElapsedTime().asMilliseconds() > shootDelay || isFirstBullet)
	{

		if (isFirstBullet) isFirstBullet = false;

		currentState = STATE_SHOOTING;

		Bullet* currentBullet = std::move(readyBullets.back());					
		currentBullet->intialize(getRotation(), getPosition());
		movingBullets.push_back(currentBullet);
		readyBullets.pop_back();

		shootTimer.restart().asMilliseconds();
	}
}

void Player::reviveBullet(const int index)
{
	std::cout << "Reviving bullet\n";
	Bullet* currentBullet = std::move(movingBullets.at(index));
	readyBullets.push_back(currentBullet);
	movingBullets.erase(movingBullets.begin() + index);
}

// Movement

void Player::movePlayer(const float dt)
{
	float angleRelavantToSprite = getRotation();
	float speed = 0.35 * dt;
	if (speedBoostTimer.getElapsedTime().asMilliseconds() < speedBostTimeRange || firstSpeedBost)
	{
		speed = boostFactor * speed;
		firstSpeedBost = false;
	}

	move(sf::Vector2f(speed * sin(angleRelavantToSprite * 3.14159265 / 180), -speed * cos(angleRelavantToSprite * 3.14159265 / 180)));
	

	if (getPosition().x > 850)		setPosition(sf::Vector2f(0, getPosition().y ));
	else if(getPosition().x < -50)	setPosition(sf::Vector2f(800, getPosition().y));
	else if (getPosition().y > 850) setPosition(sf::Vector2f(getPosition().x , 0));
	else if (getPosition().y < -50) setPosition(sf::Vector2f( getPosition().x , 800));
}

void Player::startSpeedBoost()
{
	speedBoostTimer.restart().asMilliseconds(); // start boost
	currentState = STATE_MOVING;
}

void Player::slowDown()
{
	currentState = STATE_STATIONARY;

}
void Player::moveBullets(const float dt)
{
	std::vector<int> deleteIndexes;
	int index = 0;

	for (auto* bullet : movingBullets)
	{
		bullet->move(sf::Vector2f(0.40f * dt * sin(bullet->angle * 3.14159265 / 180), -0.40f * dt * cos(bullet->angle * 3.14159265 / 180)));

		sf::Vector2f bulletPosition = bullet->getPosition();
		if (bulletPosition.x > 800 || bulletPosition.x < 0 || bulletPosition.y > 800 || bulletPosition.y < 0)
			deleteIndexes.push_back(index);

		index += 1;
	}
	for (auto index : deleteIndexes)
		reviveBullet(index);
	
}

void Player::rotatePlayer(const float dt , int direction)
{

	rotate( direction * rotationSpeed * dt);

}


// Collision

void Player::playerCollides(Asteroid* asteroid)
{
	/*
		Aproximate asteroid sprite with a circle -> fast collision detection
		check if distance to object is less than circle radius
	*/
	
	//std::cout << "Checking player collision\n";
	sf::Vector2f positition = getPosition();
	sf::Vector2f asteroidPositition = asteroid->getPosition();
	
	if (sqrt((positition.x - asteroidPositition.x) * (positition.x - asteroidPositition.x) 
			+ (positition.y - asteroidPositition.y) * (positition.y - asteroidPositition.y) < asteroid->getLocalBounds().width / 2))
	{
		if(health != 0)
			health--;
	}
}

void Player::bulletCollides(Asteroid* asteroid)
{
	for (auto* bullet : movingBullets)
	{
		sf::Vector2f positition = bullet->getPosition();
		sf::Vector2f asteroidPositition = asteroid->getPosition();

		if (sqrt((positition.x - asteroidPositition.x) * (positition.x - asteroidPositition.x)
				+ (positition.y - asteroidPositition.y) * (positition.y - asteroidPositition.y) < asteroid->getLocalBounds().width / 2))
		{
			if (asteroid->getLevel() == 1)
				score += 2;
			else if (asteroid->getLevel() == 0)
				score += 1;

			bullet->setPosition(-200.f, -200.f); 
			asteroid->downSize();
		}
	}
}

int Player::getHealth()
{
	return health;
}

int Player::getScore()
{
	return score;
}

void Player::resetParameters()
{
	score = 0;
	health = INTIAL_PLAYER_HEALTH;
}