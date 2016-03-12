#pragma once
#include "Room.h"
#include "Player.h"
class Game
{
private:
	Room _curr_room;
	Player _player;
	Monster _current_monster;
	static Game* INSTANCE;

	void dramaType(string message, int delay = 100, bool newLine = true, double freq = 130.81);
	void fight();
	void take();
public:
	Game();
	void start();
	void print_help();
	bool loop();
	Player* player();
	Monster* current_monster();
	void set_current_monster(Monster* monster);
	Room* current_room();

	static Game* instance();
};

