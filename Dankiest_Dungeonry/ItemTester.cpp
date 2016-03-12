/*
* ItemTester.cpp
* ---------------------------------------------------
* Tester for CGameItem derived classes.
*
* The student should modify the following functions:
*
*	getItemToAdd
*	add
*	remove
*
* The student should also modify the enum
*
* Suggest you start by getting the add function to work and test.
* Next, get the remove function to work.
* Next, add your own classes.
* Test your classes by adding to the array of pointers.
*
* Challenge yourself:	CHECK    Remove an item if it used up - create your own rules for this
*						NOPE     Let the player encounter obstacles and require items
*						NOPE     Further limit number of items player can have so it is harder to complete the game
*						CHECKISH Create a class the stores the array of pointers (Do not use the STL; it does not work well with pointers)
*
*/

/*
FILE: ItemTester.cpp
PROGRAMMER: Nathanael Page
DATE: 02/26/2015
PROJECT: AG121_Lab04
CREDITS: Oren Ben-Kiki, Clark Evans, Ingy döt Net for the YAML Specification, other credits given where they helped
LIMITATIONS: The game is rather small at the moment. However, there is a way to add content, so this could be remedied. In addition, the parser isn't very smart.
*/

#include "Game.h"

//int main()
//{
//	Game game;
//	game.start();
//
//	// Stay in the while until the game stops looping.
//	while (game.loop()) {}
//
//	system("pause");
//
//	return 0;
//}

#include <conio.h>
#include "Jinxes.h"

using namespace jinxes;

int main()
{
	cout << "Color " << "red " << "blue " << push_color(BRIGHT_BLUE)
		<< "red again " << push_color(BRIGHT_RED) << "white" << endl;
	_getch();
	return 0;
}