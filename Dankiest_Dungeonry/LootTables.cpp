#include "LootTables.h"
#include "CArmor.h"
#include <algorithm>

using namespace simpleyaml;
using namespace std;

const string LootTables::TABLES_FILE_LOCATION = "LootTables.yaml";
IYamlEntity* LootTables::YAML_TABLES = nullptr;
LootTable* LootTables::TABLES = nullptr;

vector<CGameItem*> LootTables::Generate(int level)
{
	if (YAML_TABLES == nullptr)
	{
		Populate();
	}

	vector<CGameItem*> drops;

	// Get the proper loot table.
	LootTable table = TABLES[level - 1];

	// Generate some potions and spells first.
	int num_pots = (rand() % table.max_potions) + 1;
	int num_spells = (rand() % table.max_spells) + 1;
	int num_weighted = (rand() % table.max_weighted) + 1;

	for (int i = 0; i < num_pots; i++)
	{
		drops.push_back(new CPotion);
	}
	for (int i = 0; i < num_spells; i++)
	{
		drops.push_back(new CSpell);
	}

	// Now get our weighted drops.
	for (int i = 0; i < num_weighted; i++)
	{
		// First, get the weight number.
		int weight_num = rand() % table.sum_weights;

		// Now loop through subtracting weights until we hit an object with a weight greater
		// than the current weight_num.
		int index = 0;
		while (index < table.num_drops && table.drops[index].weight < weight_num)
		{
			weight_num -= table.drops[index].weight;
			index++;
		}

		// And add that object to the vector.
		WeightedDrop drop = table.drops[index];
		if (drop.type == "armor")
		{
			// Find the right armor.
			uint idx = 0;
			while (idx < ArmorHandler::ARMORS.size() && ArmorHandler::ARMORS[idx].name != drop.armor_name)
			{
				idx++;
			}

			// Make one and add it to the droplist.
			CArmor* armor = new CArmor(ArmorHandler::ARMORS[idx].name, ArmorHandler::ARMORS[idx].defense);
			drops.push_back(armor);
		}
		else
		{
			// Make a weapon.
			int dice_side_counts[] = { 4, 6, 8, 10, 12, 16, 20 };
			int sel = rand() % 7;
			int dice_sides = dice_side_counts[sel];
			int num_dice = (rand() % drop.weapon_max_dice) + drop.weapon_min_dice;
			CWeapon* weapon = new CWeapon(num_dice, dice_sides);
			drops.push_back(weapon);
		}
	}

	return drops;
}

void LootTables::Populate()
{
	bool err = false;

	// Load up the list.
	YAML_TABLES = new YamlContainer(LoadYamlFromFile(TABLES_FILE_LOCATION, &err));

	// Handle errors.
	if (err)
	{
		cerr << "[CRITICAL] Error loading loot tables, the game will probably crash." << endl;
	}

	int num_tables = ToInt((*YAML_TABLES)["loot-table-count"]->value());
	TABLES = new LootTable[num_tables];

	// Now load up the loot tables from the YAML.
	for (int i = 0; i < num_tables; i++)
	{
		IYamlEntity* item = (*YAML_TABLES)["level-" + to_string(i + 1)];
		LootTable table;

		// Get the level of the table and other basic data.
		table.level = i + 1;
		table.max_potions  = ToInt((*item)["potion-max"]->value());
		table.max_spells   = ToInt((*item)["spell-max"]->value());
		table.max_weighted = ToInt((*item)["weighted-max"]->value());

		// Now read in the weighted items.
		YamlList* weighted_drops = static_cast<YamlList*>((*item)["drops"]);
		table.num_drops = weighted_drops->itemCount();
		table.sum_weights = 0;
		table.drops = new WeightedDrop[table.num_drops];

		for (int j = 0; j < table.num_drops; j++)
		{
			IYamlEntity* item = (*weighted_drops)[j];
			WeightedDrop drop;
			drop.type = (*item)["type"]->value();
			drop.weight = ToInt((*item)["weight"]->value());
			table.sum_weights += drop.weight;

			if (drop.type == "armor")
			{
				drop.armor_name = (*item)["name"]->value();
			}
			else
			{
				IYamlEntity* damage = (*item)["damage"];
				drop.weapon_min_dice = ToInt((*damage)["dice-min"]->value());
				drop.weapon_max_dice = ToInt((*damage)["dice-max"]->value());
			}

			table.drops[j] = drop;
		}
		TABLES[i] = table;
	}

	// Now we sort the tables.
	sort(TABLES, TABLES + num_tables, comptables);
}