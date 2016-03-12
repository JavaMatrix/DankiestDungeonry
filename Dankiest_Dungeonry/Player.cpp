#include "Player.h"
#include "CGameItem.h"
#include "I18n.h"
#include "Monster.h"

Player::Player() : _health(PLAYER_MAX_HEALTH), _equipped_armor(), _equipped_weapon("Candy Cane", 1, 6), _inventory_position(0)
{
	// Null out the inventory.
	for (int i = 0; i < INVENTORY_SIZE; i++)
	{
		_inventory[i] = nullptr;
	}
}

void Player::add_item(CGameItem* item_to_add)
{
	if (_inventory_position == INVENTORY_SIZE)
	{
		throw invalid_argument(I18n::Translate("inventory-full"));
	}

	_inventory[_inventory_position] = item_to_add;
	_inventory_position++;
}

CGameItem* Player::remove_item(int index)
{
	if (index < 0 || index >= _inventory_position)
	{
		char err[256];
		sprintf_s(err, I18n::Translate("inventory-no-item").c_str(), index);
		throw out_of_range(err);
	}

	CGameItem* ret = _inventory[index];
	_inventory[index] = nullptr;

	for (int i = index; i < _inventory_position - 1; i++)
	{
		_inventory[i] = _inventory[i + 1];
	}

	if (_inventory_position > 0)
	{
		_inventory[_inventory_position - 1] = nullptr;
	}
	_inventory_position--;

	return ret;
}

void Player::equip_item(int index)
{
	CGameItem* item = _inventory[index];
	if (item == nullptr)
	{
		char err[256];
		sprintf_s(err, I18n::Translate("inventory-no-item").c_str(), index + 1);
		throw out_of_range(err);
		return;
	}

	// Try casting to armor and to a weapon.
	CArmor* armor = dynamic_cast<CArmor*>(item);
	CWeapon* weapon = dynamic_cast<CWeapon*>(item);

	if (armor != nullptr)
	{
		// Swap the armor.
		CArmor t = _equipped_armor;
		_equipped_armor = *armor;
		_inventory[index] = new CArmor(t);
	}
	else if (weapon != nullptr)
	{
		// Swap the armor.
		CWeapon t = _equipped_weapon;
		_equipped_weapon = *weapon;
		_inventory[index] = new CWeapon(t);
	}
	else
	{
		char err[256];
		sprintf_s(err, I18n::Translate("inventory-non-equippable").c_str(), item->description().c_str());
		throw invalid_argument(err);
	}
}

void Player::print_inventory()
{
	cout << I18n::Translate("inventory-header") << endl;
	for (int i = 0; i < INVENTORY_SIZE; i++)
	{
		cout << "\t" << (i + 1);
		if (_inventory[i] != nullptr)
		{
			cout << " " << _inventory[i]->description() << endl;
		}
		else
		{
			cout << " <Empty>" << endl;
		}
	}
	cout << "A " << _equipped_armor.description() << endl;
	cout << "W " << _equipped_weapon.description() << endl;
}

bool Player::dead()
{
	return _health < 0;
}

int Player::damage(int dealt)
{
	if (_equipped_armor.value() > 0)
		dealt -= ((rand() % _equipped_armor.value()) + 1);

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

int Player::attack()
{
	return _equipped_weapon.value();
}

bool Player::inventory_full()
{
	return _inventory_position >= INVENTORY_SIZE;
}

int Player::health()
{
	return _health;
}

void Player::use_item(int index)
{
	if (_inventory[index] == nullptr)
	{
		throw out_of_range("");
	}
	if (_inventory[index]->use())
	{
		remove_item(index);
	}
}

void Player::heal(int& amount)
{
	_health += amount;
	if (_health > PLAYER_MAX_HEALTH)
	{
		amount -= (_health - PLAYER_MAX_HEALTH);
		_health = PLAYER_MAX_HEALTH;
	}
}