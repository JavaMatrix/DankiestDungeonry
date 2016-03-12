#pragma once
#include "SimpleYaml.h"
#include "CWeapon.h"
#include <string>

// Describes an enemy of the player
class Monster
{
public:
	static const std::string MONSTER_DEFS_FILE_NAME;
	static simpleyaml::IYamlEntity* MONSTER_DEFS;

	// Text locations in the array. Seems more sensible than having tons of string variables.
	static const int DESCRIPTION = 0;
	static const int PLURAL      = 1;
	static const int SHORT_NAME  = 2;
	static const int ATTACK_HIT  = 3;
	static const int ATTACK_MISS = 4;
	static const int DAMAGED     = 5;
	static const int DEATH       = 6;

	static const int NUMBER_TEXT_ITEMS = 7;
private:
	std::string _text[NUMBER_TEXT_ITEMS];
	Damage _damage;
	int _health;
	int _armor;
	int _loot_table;
	bool _valid;
	std::string _type;
public:
	Monster() : _valid(false) {}
	Monster(std::string type);
	int damage(int dealt);
	int attack();
	bool handleDeath(std::vector<CGameItem*>& droplist);
	std::string* text();
	bool valid();
	void invalidate();
	int health();
	int armor();
	Damage damage_info();
	std::string type();
};

