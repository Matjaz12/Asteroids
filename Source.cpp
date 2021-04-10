#include "Game.h"
#include <iostream>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

int main()
{
	Game game(SCREEN_WIDTH , SCREEN_HEIGHT);
	game.run();
	return 0;
}

