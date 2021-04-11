#include "Game.h"
#include <iostream>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

int main()
{
	/*
		Tries to create a Game object,
		if successful we run the game
	*/
	try
	{
		Game game(SCREEN_WIDTH, SCREEN_HEIGHT);
		game.run();
	}
	catch (...)
	{
		std::cerr << "Exception throw in construction of Game object\n";
	}
	return 0;
}

