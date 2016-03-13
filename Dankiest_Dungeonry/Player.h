#pragma once
#include "CGameItem.h"
#include "CArmor.h"
#include "CWeapon.h"
#include "Room.h"
#include <string>
#include <exception>

// The player class stores information about the player and game state, and provides 
// operations on that state.
class Player
{
public:
	static const int INVENTORY_SIZE = 10;
	static const int PLAYER_MAX_HEALTH = 100;
private:
	CGameItem* _inventory[INVENTORY_SIZE];
	CArmor _equipped_armor;
	CWeapon _equipped_weapon;
	int _health;
	int _inventory_position;
public:

	bool spell_buff = false;

	Player();
	// Throws an invalid_argument if the inventory is full.
	void add_item(CGameItem* item_to_add);
	// Throws an index_out_of_bounds if the given index doesn't have an item in it.
	CGameItem* remove_item(int index);
	// Throws an invalid_argument if the index doesn't have equippable gear in it.
	void equip_item(int index);
	void print_inventory();
	bool inventory_full();
	void use_item(int index);
	int health();
	bool dead();
	int damage(int dealt);
	int attack();
	void heal(int& amount);
	CArmor armor();
	CWeapon weapon();
	int count_spells();
	int count_potions();
};

