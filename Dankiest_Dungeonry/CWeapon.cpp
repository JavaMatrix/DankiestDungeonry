#include "CWeapon.h"
#include "I18n.h"
#include <string>
#include <sstream>
#include <random>
#include <time.h>

using namespace std;

typedef unsigned int uint;

int Damage::roll()
{
	int value = 0;

	// Roll each die.
	for (int i = 0; i < _numDice; i++)
	{
		// Sum up the rolls.
		value += (rand() % _diceSides) + 1;
	}

	return value;
}

string Damage::description()
{
	return to_string(_numDice) + "d" + to_string(_diceSides);
}

bool CWeapon::WEAPONS_INITIALIZED = false;
std::string CWeapon::WEAPON_NAMES[NUMBER_OF_NAMES];

CWeapon::CWeapon(string name, int numDice, int diceSides) : _name(name), _damage(numDice, diceSides)
{
	if (!WEAPONS_INITIALIZED)
	{
		for (int i = 0; i < NUMBER_OF_NAMES; i++)
		{
			WEAPON_NAMES[i] = I18n::Translate("weapon-" + to_string(i));
		}
		WEAPONS_INITIALIZED = true;
	}

	_description = _name + " " + I18n::Translate("at") + " " + _damage.description();
	_value = numDice * diceSides;
}

CWeapon::CWeapon(int numDice, int diceSides) : _damage(numDice, diceSides)
{
	if (!WEAPONS_INITIALIZED)
	{
		for (int i = 0; i < NUMBER_OF_NAMES; i++)
		{
			WEAPON_NAMES[i] = I18n::Translate("weapon-" + to_string(i));
		}
		WEAPONS_INITIALIZED = true;
	}

	// Choose a name.
	int nameChoice = (rand() % NUMBER_OF_NAMES);
	_name = string(WEAPON_NAMES[nameChoice]);

	_description = _name + " " + I18n::Translate("at") + " " + _damage.description();
	_value = numDice * diceSides;
}

CWeapon::CWeapon()
{
	if (!WEAPONS_INITIALIZED)
	{
		for (int i = 0; i < NUMBER_OF_NAMES; i++)
		{
			WEAPON_NAMES[i] = I18n::Translate("weapon-" + to_string(i));
		}
		WEAPONS_INITIALIZED = true;
	}

	// Choose a name.
	int nameChoice = (rand() % NUMBER_OF_NAMES);
	_name = string(WEAPON_NAMES[nameChoice]);

	// The user gets up to 6 dice with up to 20 faces each.
	int numDice = (rand() % 6) + 1;

	// We'll only allow real kinds of fair dice.
	int diceSideCounts[] = { 4, 6, 8, 10, 12, 16, 20 };
	int diceSides = diceSideCounts[(rand() % 7)];

	// Make the damage object.
	_damage = Damage(numDice, diceSides);

	// There's a 0.1% chance to get the ultimate weapon.
	if (rand() % 1000 == 0)
	{
		_name = I18n::Translate("super-weapon");
		_damage = Damage(42, 111);
	}

	_description = _name + " " + I18n::Translate("at") + " " + _damage.description();
	_value = numDice * diceSides;
}

bool CWeapon::use()
{
	cout << I18n::Translate("weapon-use") << endl;
	return false;
}