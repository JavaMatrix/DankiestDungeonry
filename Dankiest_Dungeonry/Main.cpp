#include "Game.h"

int main()
{
	Game game;
	game.start();

	// Stay in the while until the game stops looping.
	while (game.loop()) {}

	system("pause");

	return 0;
}