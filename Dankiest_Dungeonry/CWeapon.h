#pragma once
#include "CGameItem.h"
#include <string>

class Damage
{
private:
	int _numDice;
	int _diceSides;
public:
	Damage(int numDice, int diceSides) : _numDice(numDice), _diceSides(diceSides) {};
	Damage() {};
	std::string description();
	int roll();
};

class CWeapon : public CGameItem
{
protected:
	Damage _damage;
	std::string _name;
	static const int NUMBER_OF_NAMES = 5;
	static std::string WEAPON_NAMES[NUMBER_OF_NAMES];
	static bool WEAPONS_INITIALIZED;
public:
	CWeapon(std::string name, int numDice, int diceSides);
	CWeapon(int numDice, int diceSides);
	CWeapon();
	bool use();
};