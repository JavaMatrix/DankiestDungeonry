#include "Game.h"

#include <iostream>
#include <time.h>
#include <Windows.h>
#include <conio.h>
#include <iomanip>
#include <algorithm>

#include "SimpleYaml.h"
#include "Jinxes.h"

#include "I18n.h"
#include "CArmor.h"
#include "Player.h"
#include "LootTables.h"
#include "Utils.h"

#define STRING_COMPARE_EQUAL 0

using namespace std;
using namespace simpleyaml;
using namespace jinxes;

Game* Game::INSTANCE = nullptr;

Game::Game()
{
	INSTANCE = this;
}

void Game::start()
{
	// Fullscreen the console, from http://stackoverflow.com/questions/4053554/running-a-c-console-program-in-full-screen
	SendMessage(GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);
	SleepEx(100, true); // Wait for the window to go fullscreen.

	// Seed the random gen.
	srand((uint)time(0));

	// Make a shweet header.
	dramaType(I18n::Translate("intro-text"), 100, false);
	SleepEx(600, true);
	dramaType(I18n::Translate("ellipsis"), 300);

	cout << "    ___     ____  ____   __  _  ____    ___  _____ ______     " << endl;
	cout << "   |   \\   /    ||    \\ |  |/ ]|    |  /  _]/ ___/|      |    " << endl;
	cout << "   |    \\ |  o  ||  _  ||  ' /  |  |  /  [_(   \\_ |      |    " << endl;
	cout << "   |  D  ||     ||  |  ||    \\  |  | |    _]\\__  ||_|  |_|    " << endl;
	cout << "   |     ||  _  ||  |  ||     \\ |  | |   [_ /  \\ |  |  |      " << endl;
	cout << "   |     ||  |  ||  |  ||  .  | |  | |     |\\    |  |  |      " << endl;
	cout << "   |_____||__|__||__|__||__|\\_||____||_____| \\___|  |__|      " << endl;
	cout << "                                                               " << endl;
	cout << " ___    __ __  ____    ____    ___   ___   ____   ____   __ __ " << endl;
	cout << "|   \\  |  |  ||    \\  /    |  /  _] /   \\ |    \\ |    \\ |  |  |" << endl;
	cout << "|    \\ |  |  ||  _  ||   __| /  [_ |     ||  _  ||  D  )|  |  |" << endl;
	cout << "|  D  ||  |  ||  |  ||  |  ||    _]|  O  ||  |  ||    / |  ~  |" << endl;
	cout << "|     ||  :  ||  |  ||  |_ ||   [_ |     ||  |  ||    \\ |___, |" << endl;
	cout << "|     ||     ||  |  ||     ||     ||     ||  |  ||  .  \\|     |" << endl;
	cout << "|_____| \\__,_||__|__||___,_||_____| \\___/ |__|__||__|\\_||____/ " << endl;
	cout << "                                                               " << endl;
	cout << I18n::Translate("copywrong") << endl;
	SleepEx(1000, true);

	_curr_room = Room("field");

	print_help();
	dramaType(_curr_room.describeRoom(), 10, true, 0);
}

void Game::dramaType(string message, int delay, bool newLine, double freq)
{
	// We're gonna be beeping for 50% of the delay.
	delay /= 10;
	for (uint i = 0; i < message.length(); i++)
	{
		cout << message[i] << flush;

		if (!_kbhit())
		{
			// make a cool beep noise for letters.
			if (message[i] != ' ')
				Beep((DWORD)freq, delay * 5);
			else
				SleepEx(delay * 5, true);

			// Wait out the rest.
			SleepEx(delay * 5, true);
		}
	}

	if (newLine)
	{
		if (!_kbhit())
			SleepEx(500, true);
		cout << endl;
	}
}

void Game::print_help()
{
	cout << endl;
	cout << I18n::Translate("help-header") << endl;
	cout << I18n::Translate("help-help") << endl;
	cout << I18n::Translate("fight-help") << endl;
	cout << I18n::Translate("go-help") << endl;
	cout << I18n::Translate("take-help") << endl;
	cout << I18n::Translate("drop-help") << endl;
	cout << I18n::Translate("use-help") << endl;
	cout << I18n::Translate("inventory-help") << endl;
	cout << I18n::Translate("check-help") << endl;
	cout << I18n::Translate("equip-help") << endl;
	cout << I18n::Translate("exit-help") << endl;
}

bool starts_with(string original, string start)
{
	return original.compare(0, start.length(), start) == STRING_COMPARE_EQUAL;
}


bool Game::loop()
{

	cout << "> ";

	drawHUD();
	char cAction[256];
	cin.getline(cAction, 256);
	string action(cAction);

	Untrash(action);

	if (starts_with(action, "help"))
	{
		print_help();
		return true;
	}
	else if (starts_with(action, "exit"))
	{
		return false;
	}
	else if (starts_with(action, "fight"))
	{
		if (!_current_monster.valid())
		{
			try
			{
				_current_monster = _curr_room.pop_monster();
				cout << I18n::Translate("fight-new") << " " << _current_monster.text()[Monster::DESCRIPTION] << "." << endl;
			}
			catch (exception e)
			{
				cout << e.what() << endl;
			}
		}

		if (_current_monster.valid())
		{
			char go = '\r';
			while (go == '\r' && _current_monster.valid() && !_player.dead())
			{
				fight();
				if (_current_monster.valid() && !_player.dead())
				{
					string str = I18n::Translate("keep-fighting-prompt");
					cout << str;
					go = _getch();

					// erase the prompt
					cout << '\r';
					cout << string(str.length(), ' ');
					cout << '\r';
				}
				drawHUD();
			}
		}

		return !_player.dead() && !(!_current_monster.valid() && _current_monster.type() == "boss");
	}
	else if (starts_with(action, "auto fight"))
	{
		// Keep fighting until the room is cleared.
		while (!_player.dead() && (_current_monster.valid() || _curr_room.has_more_monsters()))
		{
			if (!_current_monster.valid())
			{
				try
				{
					_current_monster = _curr_room.pop_monster();
					cout << I18n::Translate("fight-new") << " " << _current_monster.text()[Monster::DESCRIPTION] << "." << endl;
				}
				catch (exception e)
				{
					cout << e.what() << endl;
				}
			}

			if (_current_monster.valid())
			{
				fight();
			}

			drawHUD();
		}

		if (!_player.dead())
		{
			cout << I18n::Translate("no-monsters") << endl;
		}

		return !_player.dead();
	}
	else if (starts_with(action, "take"))
	{
		take();
		return true;
	}
	else if (starts_with(action, "go"))
	{
		if (action.length() <= 3)
		{
			cout << I18n::Translate("go-where") << endl;
		}
		Room next = _curr_room.exit(Trim(action.substr(2)));

		if (next == _curr_room) return true;

		if (_current_monster.valid() || _curr_room.has_more_monsters())
		{
			cout << I18n::Translate("fight-all") << endl;
			return true;
		}

		_curr_room = next;

		// Clear the screen.
		system("cls");

		cout << _curr_room.describeRoom() << endl;
		return true;
	}
	else if (starts_with(action, "drop"))
	{
		_player.print_inventory();
		cout << I18n::Translate("drop-prompt") << " ";

		vector<CGameItem*> dropped_items;

		drawHUD();
		char idx_char[256];
		cin.getline(idx_char, 256);
		string idx = string(idx_char);
		if (idx == "all")
		{
			bool done = false;
			while (!done)
			{
				// Drop items until the function tells us we're out of items.
				try
				{
					CGameItem* item = _player.remove_item(0);
					dropped_items.push_back(item);
					char drop_msg[512];
					sprintf_s(drop_msg, I18n::Translate("dropped").c_str(), item->description().c_str());
					cout << drop_msg << endl;
				}
				catch (out_of_range e)
				{
					done = true;
				}
			}
		}
		else
		{
			int i = ToInt(idx);
			if (i == 0 || i >= Player::INVENTORY_SIZE + 1)
			{
				cout << I18n::Translate("drop-fail") << endl;
			}
			else
			{
				i--;
				try
				{
					CGameItem* item = _player.remove_item(i);
					dropped_items.push_back(item);
					char drop_msg[512];
					sprintf_s(drop_msg, I18n::Translate("dropped").c_str(), item->description().c_str());
					cout << drop_msg << endl;
				}
				catch (out_of_range e)
				{
					cout << I18n::Translate("drop-fail") << endl;
				}
			}
		}
		_curr_room.add_ground_items(dropped_items);
		return true;
	}
	else if (starts_with(action, "inv"))
	{
		cout << "HP: " << _player.health() << endl;
		_player.print_inventory();
		return true;
	}
	else if (starts_with(action, "check"))
	{
		cout << _curr_room.describeRoom() << endl;
		cout << endl;
		cout << "HP: " << _player.health() << endl;
		_player.print_inventory();
		cout << endl;
		if (_current_monster.valid())
		{
			char enemy_msg[512];
			sprintf_s(enemy_msg, I18n::Translate("enemy-check").c_str(), _current_monster.text()[Monster::DESCRIPTION].c_str(),
				_current_monster.health(), _current_monster.armor(), _current_monster.damage_info().description().c_str());
			cout << enemy_msg << endl;
		}
		return true;
	}
	else if (starts_with(action, "use"))
	{
		_player.print_inventory();
		cout << I18n::Translate("use-prompt") << " ";

		drawHUD();
		char idx_char[256];
		cin.getline(idx_char, 256);
		string idx = string(idx_char);
		int i = ToInt(idx);
		if (i == 0 || i >= Player::INVENTORY_SIZE + 1)
		{
			cout << I18n::Translate("use-fail") << endl;
		}
		else
		{
			try
			{
				_player.use_item(i - 1);
			}
			catch (exception e)
			{
				cout << I18n::Translate("use-fail") << endl;
			}
		}
		return true;
	}
	else if (starts_with(action, "equip"))
	{
		_player.print_inventory();
		cout << I18n::Translate("equip-prompt") << " ";

		drawHUD();
		char idx_char[256];
		cin.getline(idx_char, 256);
		string idx = string(idx_char);
		int i = ToInt(idx);
		if (i == 0 || i >= Player::INVENTORY_SIZE + 1)
		{
			cout << I18n::Translate("equip-fail") << endl;
		}
		else
		{
			i--;
			try
			{
				_player.equip_item(i);
				cout << I18n::Translate("equip-success") << endl;
			}
			catch (out_of_range e)
			{
				cout << e.what() << endl;
			}
			catch (invalid_argument e)
			{
				cout << e.what() << endl;
			}
		}
		return true;
	}
	else if (starts_with(action, "clear"))
	{
		system("cls");
		return true;
	}
	else if (starts_with(action, "i18n"))
	{
		try
		{
			// Take off the "i18n ".
			action = action.substr(5);
			int idx = action.find(' ');

			// Replace spaces with dashes.
			while (idx != string::npos)
			{
				action.replace(idx, 1, "-");
				idx = action.find(' ');
			}

			// Find a translation.
			cout << I18n::Translate(action) << endl;
		}
		catch (exception e)
		{
			// No translations available.
			cout << I18n::Translate("no-i18n") << endl;
		}

		return true;
	}
	else if (starts_with(action, "reload"))
	{
		// Reload the resource files.
		I18n::Initialize();
		ArmorHandler::LoadArmors();
		LootTables::Populate();
		Monster::MONSTER_DEFS = nullptr;
		Room::ROOM_DEFS = nullptr;
		_curr_room.reload_monsters();
		cout << I18n::Translate("reload-success") << endl;
		return true;
	}

	int idx = action.find(' ');
	while (idx != string::npos)
	{
		action.replace(idx, 1, "-");
		idx = action.find(' ');
	}

	try
	{
		string text = I18n::Translate("command-" + action, true);
		if (text == "ERRNO")
			cout << I18n::Translate("invalid-command") << endl;
		else
			cout << text << endl;
	}
	catch (exception e)
	{
		cout << I18n::Translate("invalid-command") << endl;
	}

	// Redraw it.
	drawHUD();

	return true;
}

void Game::fight()
{
	// Fixed 1/20th chance to miss when not buffed.
	if (rand() % 20 == 0 && !_player.spell_buff)
	{
		// Tell the player they missed.
		char miss_msg[512];
		sprintf_s(miss_msg, I18n::Translate("player-miss").c_str(), _current_monster.text()[Monster::SHORT_NAME].c_str());
		cout << miss_msg << endl;
	}
	else
	{
		int damage = _player.attack();
		if (_player.spell_buff)
		{
			damage *= CSpell::SPELL_DAMAGE_MUL;
			// In addition to the multiplier, we add some extra so the numbers don't look weird.
			damage += rand() % CSpell::SPELL_DAMAGE_MUL;
			_player.spell_buff = false;
		}
		damage = _current_monster.damage(damage);

		// Tell the player they hit.
		if (damage > 0)
		{
			char hit_msg[512];
			sprintf_s(hit_msg, I18n::Translate("player-hit").c_str(), _current_monster.text()[Monster::SHORT_NAME].c_str(), damage);
			cout << hit_msg << endl;
		}
		else
		{
			char hit_msg[512];
			sprintf_s(hit_msg, I18n::Translate("monster-blocked").c_str(), _current_monster.text()[Monster::SHORT_NAME].c_str());
			cout << hit_msg << endl;
		}

		// Tell the player the monster was damaged or killed.
		vector<CGameItem*> drops;
		if (!_current_monster.handleDeath(drops))
		{
			if (damage > 0)
			{
				char damage_msg[512];
				sprintf_s(damage_msg, I18n::Translate("monster-damage").c_str(), _current_monster.text()[Monster::SHORT_NAME].c_str(),
					_current_monster.text()[Monster::DAMAGED].c_str());
				cout << damage_msg << endl;
			}
		}
		else
		{
			// Tell the player the monster is dead.
			char death_msg[512];
			const char* name = _current_monster.text()[Monster::SHORT_NAME].c_str();
			sprintf_s(death_msg, I18n::Translate("monster-death").c_str(), name,
				_current_monster.text()[Monster::DEATH].c_str(), name);

			cout << death_msg << endl;

			// List the monster's drops.
			int pots = 0, spells = 0;
			for each (CGameItem* drop in drops)
			{
				if (dynamic_cast<CPotion*>(drop) != nullptr)
				{
					pots++;
				}
				else if (dynamic_cast<CSpell*>(drop) != nullptr)
				{
					spells++;
				}
				else
				{
					char drop_msg[512];
					sprintf_s(drop_msg, I18n::Translate("item-drop").c_str(), name, drop->description().c_str());
					cout << drop_msg << endl;
				}
			}
			if (pots > 0)
			{
				char drop_msg[512];
				if (pots == 1)
				{
					sprintf_s(drop_msg, I18n::Translate("item-drop").c_str(), name, CPotion().description().c_str());
				}
				else
				{
					sprintf_s(drop_msg, I18n::Translate("many-drop").c_str(), name, pots, CPotion().description().c_str());
				}
				cout << drop_msg << endl;
			}
			if (spells > 0)
			{
				char drop_msg[512];
				if (spells == 1)
				{
					sprintf_s(drop_msg, I18n::Translate("item-drop").c_str(), name, CSpell().description().c_str());
				}
				else
				{
					sprintf_s(drop_msg, I18n::Translate("many-drop").c_str(), name, spells, CSpell().description().c_str());
				}
				cout << drop_msg << endl;
			}

			_curr_room.add_ground_items(drops);

			// If this was the boss mob, give a game win.
			if (_current_monster.type() == "boss")
			{
				dramaType(I18n::Translate("game-win"), 10);
				dramaType("You win!", 300);
			}

			// Tell the game we need another monster.
			_current_monster.invalidate();
		}
	}

	// Now it's the monster's turn.
	if (_current_monster.valid())
	{
		const char* name = _current_monster.text()[Monster::SHORT_NAME].c_str();

		// Fixed 1/20th chance to miss.
		if (rand() % 20 == 0)
		{
			// Tell the player the monster missed.
			char miss_msg[512];
			sprintf_s(miss_msg, I18n::Translate("monster-miss").c_str(), name, _current_monster.text()[Monster::ATTACK_MISS].c_str());
			cout << miss_msg << endl;
		}
		else
		{
			// Apply the damage.
			int damage = _player.damage(_current_monster.attack());

			// Tell the player the monster hit them.
			char hit_msg[512];
			sprintf_s(hit_msg, I18n::Translate("monster-hit").c_str(), name, _current_monster.text()[Monster::ATTACK_HIT].c_str());
			cout << hit_msg << endl;

			if (damage > 0)
			{
				char dmg_msg[512];
				sprintf_s(dmg_msg, I18n::Translate("player-damaged").c_str(), damage);
				cout << dmg_msg << endl;
			}
			else
			{
				char dmg_msg[512];
				sprintf_s(dmg_msg, I18n::Translate("player-blocked").c_str(), _current_monster.text()[Monster::SHORT_NAME].c_str());
				cout << dmg_msg << endl;
			}

			if (_player.dead())
			{
				// Tell the player that they died.
				dramaType(I18n::Translate("player-died"), 100);
				cout << endl;
				dramaType(I18n::Translate("game-over"), 300);
				SleepEx(1000, true);
			}
		}
	}
}

void Game::take()
{
	if (_player.inventory_full())
	{
		cout << I18n::Translate("inventory-full") << endl;
		return;
	}
	else
	{
		uint sz = _curr_room.ground_items().size();
		int pots = 0, spells = 0;

		map<int, int> choices;
		int loc = 1;
		for (uint i = 0; i < sz; i++)
		{
			CGameItem* item = _curr_room.ground_items()[i];
			if (dynamic_cast<CSpell*>(item) != nullptr) spells++;
			else if (dynamic_cast<CPotion*>(item) != nullptr) pots++;
			else
			{
				choices[loc] = i;
				cout << loc << " " << item->description() << endl;
				loc++;
			}
		}
		if (pots > 0)
		{
			cout << "P " << pots << " " << CPotion().description();
			if (pots > 1)
				cout << "s";
			cout << endl;
		}
		if (spells > 0)
		{
			cout << "S " << spells << " " << CSpell().description();
			if (pots > 1)
				cout << "s";
			cout << endl;
		}

		cout << I18n::Translate("take-prompt") << " ";

		drawHUD();
		char take_what_char[256];
		cin.getline(take_what_char, 256);
		string take_what = string(take_what_char);
		Untrash(take_what);
		while (take_what != "done" && !_player.inventory_full() && !_curr_room.ground_items().empty())
		{
			if (take_what == "all")
			{
				while (_curr_room.ground_items().size() > 0 && !_player.inventory_full())
				{
					CGameItem* item = _curr_room.pop_ground_item(0);
					_player.add_item(item);
					char taken_msg[512];
					sprintf_s(taken_msg, I18n::Translate("taken").c_str(), item->description().c_str());
					cout << taken_msg << endl;
				}
			}
			else if (starts_with(take_what, "p"))
			{
				int amount = 1;
				if (take_what.length() > 1)
				{
					int num = ToInt(take_what.substr(1));
					if (num > 0) amount = num;
				}

				if (amount > pots)
					amount = pots;

				bool go = true;
				int i = 0;
				while (i < amount && go)
				{
					// Find the first amount potions and grab them.
					uint j = 0;
					bool found = false;
					while (!found && j < _curr_room.ground_items().size())
					{
						if (dynamic_cast<CPotion*>(_curr_room.ground_items()[j]) != nullptr)
						{
							CGameItem* item = _curr_room.pop_ground_item(j);
							try
							{
								_player.add_item(item);
							}
							catch (invalid_argument e)
							{
								cout << e.what() << endl;
								amount = i + 1;
								go = false;
							}
							found = true;
						}
						j++;
					}
					i++;
				}
				char taken_msg[512];
				string item_desc = CPotion().description();
				if (amount != 1) item_desc += "s";

				if (amount != 1)
					sprintf_s(taken_msg, I18n::Translate("taken-many").c_str(), amount, item_desc.c_str());
				else
					sprintf_s(taken_msg, I18n::Translate("taken").c_str(), item_desc.c_str());

				cout << taken_msg << endl;
			}
			else if (starts_with(take_what, "s"))
			{
				int amount = 1;
				if (take_what.length() > 1)
				{
					int num = ToInt(take_what.substr(1));
					if (num > 0) amount = num;
				}

				if (amount > spells)
					amount = spells;

				bool go = true;
				int i = 0;
				while (i < amount && go)
				{
					// Find the first amount spells and grab them.
					uint j = 0;
					bool found = false;
					while (!found && j < _curr_room.ground_items().size())
					{
						if (dynamic_cast<CSpell*>(_curr_room.ground_items()[j]) != nullptr)
						{
							CGameItem* item = _curr_room.pop_ground_item(j);
							try
							{
								_player.add_item(item);
							}
							catch (invalid_argument e)
							{
								cout << e.what() << endl;
								go = false;
								amount = i + 1;
							}
							found = true;
						}
						j++;
					}
					i++;
				}
				char taken_msg[512];
				string item_desc = CSpell().description();
				if (amount != 1) item_desc += "s";

				if (amount != 1)
					sprintf_s(taken_msg, I18n::Translate("taken-many").c_str(), amount, item_desc.c_str());
				else
					sprintf_s(taken_msg, I18n::Translate("taken").c_str(), item_desc.c_str());

				cout << taken_msg << endl;
			}
			else
			{
				uint idx = (uint)ToInt(take_what);
				if (idx == 0 || choices.count(idx) == 0)
				{
					cout << I18n::Translate("take-fail") << endl;
				}
				else
				{
					idx = choices[idx];
					CGameItem* item = _curr_room.pop_ground_item(idx);
					_player.add_item(item);
					char taken_msg[512];
					sprintf_s(taken_msg, I18n::Translate("taken").c_str(), item->description().c_str());
					cout << taken_msg << endl;
				}
			}
			pots = 0;
			spells = 0;
			sz = _curr_room.ground_items().size();
			choices.clear();
			loc = 1;
			for (uint i = 0; i < sz; i++)
			{
				CGameItem* item = _curr_room.ground_items()[i];
				if (dynamic_cast<CSpell*>(item) != nullptr) spells++;
				else if (dynamic_cast<CPotion*>(item) != nullptr) pots++;
				else
				{
					choices[loc] = i;
					cout << loc << " " << item->description() << endl;
					loc++;
				}
			}

			if (pots > 0)
			{
				cout << "P " << pots << " " << CPotion().description();
				if (pots > 1)
					cout << "s";
				cout << endl;
			}
			if (spells > 0)
			{
				cout << "S " << spells << " " << CSpell().description();
				if (pots > 1)
					cout << "s";
				cout << endl;
			}

			if (!_player.inventory_full() && !_curr_room.ground_items().empty())
			{
				drawHUD();
				cout << I18n::Translate("take-prompt") << " ";
				cin.getline(take_what_char, 256);
				take_what = string(take_what_char);
				Untrash(take_what);
			}

			if (_player.inventory_full())
			{
				cout << I18n::Translate("inventory-full") << endl;
			}
		}
	}
}

Player* Game::player()
{
	return &_player;
}

Monster* Game::current_monster()
{
	return &_current_monster;
}

void Game::set_current_monster(Monster* monster)
{
	_current_monster = *monster;
}

Game* Game::instance()
{
	return INSTANCE;
}

Room* Game::current_room()
{
	return &_curr_room;
}

void Game::drawHUD()
{
	WinConsole::MoveCursorTo(0, 0);

	// Start by drawing the outline.
	set_color_all(Colors::GREY);
	cout << SpecialChars::DoubleBorder::TopLeftCorner
		<< string(SIZE_FULLSCREEN - 2, SpecialChars::DoubleBorder::HorizontalSide)
		<< SpecialChars::DoubleBorder::TopRightCorner;
	cout << SpecialChars::DoubleBorder::VerticalSide
		<< string(SIZE_FULLSCREEN - 2, ' ')
		<< SpecialChars::DoubleBorder::VerticalSide;
	cout << SpecialChars::DoubleBorder::BottomLeftCorner
		<< string(SIZE_FULLSCREEN - 2, SpecialChars::DoubleBorder::HorizontalSide)
		<< SpecialChars::DoubleBorder::BottomRightCorner;
	reset_color_all();

	// Now draw the health bar.
	WinConsole::MoveCursorTo(2, 1);
	std::string health_text = "Health: " + to_string(_player.health()) + "/100";
	for (int i = 0; i < 50; i++)
	{
		// Choose a background color.
		Color background = Colors::RED;
		if (i > _player.health() / 2)
		{
			background = Colors::BLACK;
		}

		// Print the health text or a quarter-filled box to create the health bar effect.
		if (i < health_text.size())
		{
			cout << set_color(health_text[i], Colors::WHITE, background);
		}
		else
		{
			cout << set_color(SpecialChars::Boxes::QuarterFull, Colors::RED, background);
		}
	}

	cout << "  ";

	// Print some stats.
	cout << "ATK: " << setw(5) << left << _player.weapon().value() << "  ";
	cout << "DEF: " << setw(5) << left << _player.armor().value() << "  ";
	cout << "POT: " << setw(5) << left << _player.count_potions() << "  ";
	cout << "SPL: " << setw(5) << left << _player.count_spells() << "  ";

	// Show the spell buff
	if (_player.spell_buff)
	{
		// Show the buff.
		cout << set_color(I18n::Translate("hud-buff"), Colors::BRIGHT_YELLOW);
	}
	else
	{
		// Or erase it.
		cout << set_color(string(I18n::Translate("hud-buff").length(), SpecialChars::Boxes::Full), Colors::BLACK);
	}

	WinConsole::ReturnCursorToHome();
}