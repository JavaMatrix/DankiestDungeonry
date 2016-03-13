#include "Room.h"

#include <string>
#include <algorithm>
#include <iostream>

#include "SimpleYaml.h"
#include "Color.h"

#include "I18n.h"
#include "Utils.h"

using namespace std;
using namespace simpleyaml;
using namespace jinxes;

const string Room::ROOM_DEFS_FILE_NAME= "Rooms.yaml";
IYamlEntity* Room::ROOM_DEFS = nullptr;

Room::Room(string name)
{
	if (ROOM_DEFS == nullptr)
	{
		bool err;
		ROOM_DEFS = new YamlContainer(LoadYamlFromFile(ROOM_DEFS_FILE_NAME, &err));
		if (err)
			cerr << "ERROR: Couldn't load room definitions. Game will likely crash." << endl;
	}

	// Load up the room definition.
	IYamlEntity* yaml = (*ROOM_DEFS)[name];
	_name = yaml->key();
	_description = (*yaml)["description"]->value();

	// Get the list of monsters.
	YamlList* monsters = static_cast<YamlList*>((*yaml)["monsters"]);
	for (int i = 0; i < monsters->itemCount(); i++)
	{
		// In each definition is a monster type and a number of monsters.
		IYamlEntity* monsterDef = (*monsters)[i];
		string type = (*monsterDef)["type"]->value();
		int count = ToInt((*monsterDef)["count"]->value());

		// Put count of type in the monster vector.
		for (int j = 0; j < count; j++)
		{
			_monsters.push_back(Monster(type));
		}
	}

	// Scramble the monster's order.
	random_shuffle(_monsters.begin(), _monsters.end());

	// Now load up the exits.
	YamlList* exits = static_cast<YamlList*>((*yaml)["exits"]);
	for (int i = 0; i < exits->itemCount(); i++)
	{
		// In each definition is a direction and a room tag.
		IYamlEntity* exitDef = (*exits)[i];
		string direction = (*exitDef)["direction"]->value();
		string destination = (*exitDef)["to"]->value();
		_exits[direction] = destination;
		Untrash(direction);
		_alt_exits[direction] = destination;
	}
}

void Room::describeRoom()
{
	if (ROOM_DEFS == nullptr)
	{
		bool err;
		ROOM_DEFS = new YamlContainer(LoadYamlFromFile(ROOM_DEFS_FILE_NAME, &err));
		if (err)
			cerr << "ERROR: Couldn't load room definitions. Game will likely crash." << endl;
	}

	cout << I18n::Translate("you-are-in") << " " << _description << "." << endl;

	map<string, int> monster_counts;
	map<string, Monster> monster_samples;

	// Describe the monsters.
	// PRAISE STROUSTRUP FOR C++11
	for each (Monster m in _monsters)
	{
		monster_counts[m.text()[Monster::SHORT_NAME]]++;
		monster_samples[m.text()[Monster::SHORT_NAME]] = m;
	}

	for each (pair<string, int> kvp in monster_counts)
	{
		cout << I18n::Translate("there-is");
		if (kvp.second > 1)
		{
			// Ex. a flock of 25 cyborg chickadees
			char multipleDescription[256];
			sprintf_s(multipleDescription, monster_samples[kvp.first].text()[Monster::PLURAL].c_str(), kvp.second);
			cout << " " << set_color(multipleDescription, Colors::RED);
		}
		else
		{
			// Ex. a robotic chickadee with malevolent programming
			cout << " " << set_color(monster_samples[kvp.first].text()[Monster::DESCRIPTION], Colors::RED);
		}
		cout << " " << I18n::Translate("here") << "." << endl;
	}

	// Describe ground items.
	int pots = 0, spells = 0;
	for each (CGameItem* item_ptr in _ground_items)
	{
		if (dynamic_cast<CSpell*>(item_ptr) != nullptr) spells++;
		else if (dynamic_cast<CPotion*>(item_ptr) != nullptr) pots++;
		else
		{
			cout << I18n::Translate("there-is") << " " << I18n::Translate("a") << " "
				<< set_color(item_ptr->description(), Colors::AQUA) << " " << I18n::Translate("on-ground") << "." << endl;
		}
	}
	if (pots > 0)
	{
		if (pots > 1)
			cout << I18n::Translate("there-are") << " " << pots;
		else
			cout << I18n::Translate("there-is") << " " << I18n::Translate("a");
		cout << " " << set_color(CPotion().description(), Colors::PURPLE);
		if (pots > 1)
			cout << set_color("s", Colors::PURPLE);
		cout << " " << I18n::Translate("on-ground") << "." << endl;
	}
	if (spells > 0)
	{
		if (spells > 1)
			cout << I18n::Translate("there-are") << " " << spells;
		else
			cout << I18n::Translate("there-is") << " " << I18n::Translate("a");
		cout << " " << set_color(CSpell().description(), Colors::YELLOW);
		if (spells > 1)
			cout << set_color("s", Colors::YELLOW);
		cout << " " << I18n::Translate("on-ground") << "." << endl;
	}

	// Describe exits.
	for each (pair<string, string> exit in _exits)
	{
		char ex_msg[256];
		string room_name = (*(*ROOM_DEFS)[exit.second])["description"]->value();
		exit.first[0] = toupper(exit.first[0]);
		sprintf_s(ex_msg, I18n::Translate("exit-desc").c_str(), exit.first.c_str(), room_name.c_str());
		cout << ex_msg << endl;
	}
}

Room Room::exit(std::string direction)
{
	if (_exits.count(direction) == 0)
	{
		if (_alt_exits.count(direction) == 0)
		{
			cout << I18n::Translate("no-exit") << endl;
			return *this;
		}
		return _alt_exits[direction];
	}
	return _exits[direction];
}

Monster Room::pop_monster()
{
	if (_monsters.empty()) throw out_of_range(I18n::Translate("no-monsters"));
	Monster ret = _monsters.back();
	_monsters.pop_back();
	return ret;
}

bool Room::has_more_monsters()
{
	return _monsters.size() > 0;
}

bool Room::operator==(Room& other)
{
	return _name == other._name;
}

vector<CGameItem*> Room::ground_items()
{
	return _ground_items;
}

void Room::add_ground_items(vector<CGameItem*> drops)
{
	for each (CGameItem* item in drops)
	{
		_ground_items.push_back(item);
	}
}

CGameItem* Room::pop_ground_item(int index)
{
	CGameItem* ret = _ground_items[index];
	_ground_items.erase(_ground_items.begin() + index);
	return ret;
}

void Room::reload_monsters()
{
	vector<Monster> monsters;
	for each (Monster m in _monsters)
	{
		monsters.push_back(Monster(m.type()));
	}
	_monsters = monsters;
}