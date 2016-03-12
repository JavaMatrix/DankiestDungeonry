#pragma once
#include "CGameItem.h"
#include <string>
#include <vector>

class CArmor :
	public CGameItem
{
private:
	std::string _name;
	int _bonus;
	static const int MAX_BONUS = 10;
	static bool LOADED_SUFFIXES;
	static std::string SUFFIXES[];
public:
	CArmor();
	CArmor(std::string name, int def);
	bool use();
};

struct ArmorData
{
	std::string name;
	int defense;
};

class ArmorHandler
{
private:
	static const std::string FILE_NAME;
public:
	static std::vector<ArmorData> ARMORS;
	static void LoadArmors();
};

