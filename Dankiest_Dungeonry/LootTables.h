#pragma once
#include "SimpleYaml.h"
#include "CGameItem.h"
#include "CWeapon.h"

#include <vector>

struct WeightedDrop
{
	std::string type;
	int weight;

	std::string armor_name;

	int weapon_max_dice;
	int weapon_min_dice;
};

struct LootTable
{
	WeightedDrop* drops;
	int num_drops;
	int sum_weights;
	int max_potions;
	int max_spells;
	int max_weighted;
	int level;
};

inline bool comptables(const LootTable& first, const LootTable& second)
{
	return first.level < second.level;
}

class LootTables
{
private:
	static const std::string TABLES_FILE_LOCATION;
	static simpleyaml::IYamlEntity* YAML_TABLES;
	static LootTable* TABLES;

public:
	static std::vector<CGameItem*> Generate(int level);
	static void Populate();
};

