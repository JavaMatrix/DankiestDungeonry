#pragma once
#include "CGameItem.h"
#include "I18n.h"
#include "Game.h"

// CSpell
CSpell::CSpell()
{
	_value = 5;
	_description = "Spell";
}

CSpell::CSpell(CSpell & right)
{
	_value = right._value;
	_description = right._description;
}

bool CSpell::use()
{
	if (Game::instance()->player()->spell_buff)
	{
		cout << I18n::Translate("spell-fail") << endl;
		return false;
	}
	Game::instance()->player()->spell_buff = true;
	cout << I18n::Translate("spell-use") << endl;
	return true;
}


// CPotion
CPotion::CPotion()

{
	_value = 2;
	_description = "Potion";
}

CPotion::CPotion(CPotion  & right)
{
	_value = right._value;
	_description = right._description;
}

bool CPotion::use()
{
	// Heal 1-10 health.
	int amount = rand() % 10 + 1;

	if (Game::instance()->player()->spell_buff)
	{
		amount *= 3;
		amount += rand() % 3;
	}

	Game::instance()->player()->heal(amount);

	if (Game::instance()->player()->spell_buff)
	{
		char heal_msg[512];
		sprintf_s(heal_msg, I18n::Translate("crit-heal").c_str(), description().c_str(), amount, Game::instance()->player()->health());
		cout << heal_msg << endl;
		Game::instance()->player()->spell_buff = false;
	}
	else
	{
		char heal_msg[512];
		sprintf_s(heal_msg, I18n::Translate("heal").c_str(), description().c_str(), amount, Game::instance()->player()->health());
		cout << heal_msg << endl;
	}

	return true;
}