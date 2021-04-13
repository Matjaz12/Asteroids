#include "Player.h"

Player::Player(const sf::Texture& _playerTexture, const sf::Texture& _bulletTexture)
	:
	playerTexture{ _playerTexture },
	bulletTexture{_bulletTexture },
	health{ INTIAL_PLAYER_HEALTH},
	score{ 0 }
{
	setUp();
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

void Player::setUp()
{
	/*
	Function initializes player sprite
	*/
	setTexture(playerTexture);
	scale(sf::Vector2f(0.015f, 0.025f));
	setOrigin(400.0f + getLocalBounds().width / 2, 400.f + getLocalBounds().height / 2);
	setPosition(400.0f, 400.0f);
}

// Shooting
void Player::createBullets()
{
	for (int i{ 0 }; i < numberOfBullets; i++)
		readyBullets.push_back(new Bullet(bulletTexture));
}

void Player::shootBullet()
{
	/*
	Function checks if elapsed time since last bullet shot is greater than specified shootDelay
	it adds the bullet pointer to the moving bullets dequeu and restarts the shotTimer
	*/
	if (shotTimer.getElapsedTime().asMilliseconds() > shootDelay || isFirstBullet)
	{

		if (isFirstBullet) isFirstBullet = false;

		currentState = STATE_SHOOTING;

		Bullet* currentBullet = std::move(readyBullets.back());					
		currentBullet->intialize(getRotation(), getPosition());
		movingBullets.push_back(currentBullet);
		readyBullets.pop_back();

		shotTimer.restart().asMilliseconds();
	}
}

void Player::reviveBullet(const int index)
{
	/*
	Function moves the bullet pointer from movingBullets to
	readyBullets array
	*/
	Bullet* currentBullet = std::move(movingBullets.at(index));
	readyBullets.push_back(currentBullet);
	movingBullets.erase(movingBullets.begin() + index);
}

// Movement

void Player::movePlayer(const float dt)
{
	/*
	Function moves the player in direction to which the player sprite points to
	,it calucalates the speed at which the player moves.
	The speed is a function of elapsedTimeSinceTakeoff ( time since the user first pressed the UP Arrow Key)
	*/

	float angleRelavantToSprite = getRotation();
	float speed = 0.35 * dt;
	float elapsedTimeSinceTakeoff = thrustTimer.getElapsedTime().asMilliseconds();
	
	if ( elapsedTimeSinceTakeoff < speedBostTimeRange || firstSpeedBost)
	{
		if (elapsedTimeSinceTakeoff != 0)
		{
			// deltaSpeed is a simple linear interpolation defined in range t(0ms,speedBostTimeRangems) with values clamped between (deltaSpeedMax, deltaSpeedMin)
			// with a slope < 0 && |slope| < 1 therefor deltaSpeed function decreases as a function of elapsedTimeSinceTakeoff
			// which roughly models the thurst of a rocket 
			const float deltaSpeedMax = 6.f;
			const float deltaSpeedMin = 1.f;
			const float slope = ((deltaSpeedMax - deltaSpeedMin) / (0.0f - speedBostTimeRange));
			float deltaSpeed = deltaSpeedMax + slope * elapsedTimeSinceTakeoff;
			speed += deltaSpeed;
		}
		firstSpeedBost = false;
	}

	move(sf::Vector2f(speed * sin(angleRelavantToSprite * 3.14159265 / 180), -speed * cos(angleRelavantToSprite * 3.14159265 / 180))); // moves the player in correct direction

	// Bounds Check
	if (getPosition().x > 850)		setPosition(sf::Vector2f(0, getPosition().y ));
	else if(getPosition().x < -50)	setPosition(sf::Vector2f(800, getPosition().y));
	else if (getPosition().y > 850) setPosition(sf::Vector2f(getPosition().x , 0));
	else if (getPosition().y < -50) setPosition(sf::Vector2f( getPosition().x , 800));
}

void Player::startRocketThrustTimer()
{
	/*
	Function gets called by the Game object when player presses the UP Arrow key
	it starts the thrustTimer clock and changes the player state
	*/
	thrustTimer.restart().asMilliseconds(); // start boost
	currentState = STATE_MOVING;
}

void Player::slowDown()
{
	currentState = STATE_STATIONARY;
}

void Player::moveBullets(const float dt)
{
	/*
	Function moves bullets at an angle, at which the bullet was shot
	*/

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
	Collision check between player and asteroid
	*/
	if (getGlobalBounds().intersects(asteroid->getGlobalBounds()))
	{
		if (health != 0 && asteroid->madeDamage == false)
		{
			health--;
			asteroid->madeDamage = true;
		}
	}
	else if (asteroid->madeDamage == true)
		asteroid->madeDamage = false;
}

void Player::bulletCollides(Asteroid* asteroid)
{
	/*
	Collision check between movingBullets and asteroid
	*/

	for (auto* bullet : movingBullets)
	{
		sf::Vector2f positition = bullet->getPosition();
		sf::Vector2f asteroidPositition = asteroid->getPosition();


		if (bullet->getGlobalBounds().intersects(asteroid->getGlobalBounds()))
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

// S&G
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