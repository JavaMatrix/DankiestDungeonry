#include "Monster.h"
#include "SimpleYaml.h"
#include "LootTables.h"

using namespace simpleyaml;
using namespace std;

const string Monster::MONSTER_DEFS_FILE_NAME = "Monsters.yaml";
IYamlEntity* Monster::MONSTER_DEFS = nullptr;

Monster::Monster(string type) : _valid(true)
{
	if (MONSTER_DEFS == nullptr)
	{
		bool err;
		MONSTER_DEFS = new YamlContainer(LoadYamlFromFile(MONSTER_DEFS_FILE_NAME, &err));
		if (err)
			cerr << "ERROR: Couldn't load monster definitions. Game will likely crash." << endl;
	}

	// Load up the monster definition.
	IYamlEntity* yaml = (*MONSTER_DEFS)[type];
	_type = type;

	// Read in all of the text segments.
	_text[DESCRIPTION] = (*yaml)["description"]->value();
	_text[PLURAL] = (*yaml)["plural"]->value();
	_text[SHORT_NAME] = (*yaml)["short"]->value();
	_text[ATTACK_HIT] = (*yaml)["attack-hit-text"]->value();
	_text[ATTACK_MISS] = (*yaml)["attack-miss-text"]->value();
	_text[DAMAGED] = (*yaml)["damaged-text"]->value();
	_text[DEATH] = (*yaml)["death-text"]->value();

	// Create the damage object.
	int numDice = ToInt((*(*yaml)["damage"])["dice"]->value());
	int diceSides = ToInt((*(*yaml)["damage"])["sides"]->value());
	_damage = Damage{ numDice, diceSides };

	// Get the health stats.
	_health = ToInt((*yaml)["health"]->value());
	_armor = ToInt((*yaml)["armor"]->value());

	// Finally, get the loot table.
	_loot_table = ToInt((*yaml)["max-drop-level"]->value());
}

int Monster::damage(int dealt)
{
	if (_armor > 0)
		dealt -= ((rand() % _armor) + 1);

	if (dealt <= 0)
	{
		return 0;
	}
	else
	{
		_health -= dealt;
		return dealt;
	}
}

int Monster::attack()
{
	return _damage.roll();
}

bool Monster::handleDeath(vector<CGameItem*>& droplist)
{
	if (_health > 0) return false;

	int loot_level = (rand() % _loot_table) + 1;
	vector<CGameItem*> drops = LootTables::Generate(loot_level);
	for each (CGameItem* drop in drops)
	{
		droplist.push_back(drop);
	}

	return true;
}

string* Monster::text()
{
	return _text;
}

bool Monster::valid()
{
	return _valid;
}

void Monster::invalidate()
{
	_valid = false;
}

int Monster::health()
{
	return _health;
}

int Monster::armor()
{
	return _armor;
}

Damage Monster::damage_info()
{
	return _damage;
}

string Monster::type()
{
	return _type;
}