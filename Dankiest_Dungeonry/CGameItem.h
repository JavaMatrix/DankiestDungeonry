/*
* CGameItem.h
*----------------------------------------------------
* Definition of item class and all derived classes.
*
* The student should add derived class definitions here.
*
*/



#pragma once
#include <iostream>
#include <string>
// We're gonna define other game objects outside of this header, but including them here
// will allow users to just include this class and get everything they need.

using namespace std;

class CGameItem {
protected: // can be seen by derived classes 

	int      _value;
	string   _description;

public:

	CGameItem()
	{
		_value = 0;
	}

	~CGameItem()
	{
	}

	virtual int value() { return _value; }
	string description() { return _description; }

	// pure virtual function, everyone must override
	virtual bool use() = 0;

};

// A little shortcut for more readable code.
typedef CGameItem* GameItemPtr;

class CSpell : public CGameItem{
public:
	static const int SPELL_DAMAGE_MUL = 3;

	CSpell();

	CSpell(CSpell & right);

	bool use();

};

class CPotion : public CGameItem {
public:

	CPotion();

	CPotion(CPotion& right);

	bool use();
};