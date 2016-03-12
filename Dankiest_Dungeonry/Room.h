#pragma once
#include <string>
#include <vector>
#include <map>
#include "SimpleYaml.h"
#include "Monster.h"

class Room
{
private:

	std::string _name;
	std::string _description;
	std::vector<Monster> _monsters;
	std::map<std::string, std::string> _exits;
	std::map<std::string, std::string> _alt_exits;
	std::vector<CGameItem*> _ground_items;

public:
	static const std::string ROOM_DEFS_FILE_NAME;
	static simpleyaml::IYamlEntity* ROOM_DEFS; 
		
	Room() {}
	Room(std::string name);
	Room exit(std::string direction);
	std::string describeRoom();
	Monster pop_monster();
	bool has_more_monsters();
	bool operator==(Room& other);
	std::vector<CGameItem*> ground_items();
	void add_ground_items(vector<CGameItem*> drops);
	CGameItem* pop_ground_item(int index);
	void reload_monsters();
};