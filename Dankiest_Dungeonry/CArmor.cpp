#include "CArmor.h"
#include "SimpleYaml.h"
#include "I18n.h"
#include <random>
#include <time.h>

using namespace std;
using namespace simpleyaml;

bool CArmor::LOADED_SUFFIXES = false;
string CArmor::SUFFIXES[MAX_BONUS];

CArmor::CArmor(string name, int def) : _name(name)
{
	if (!LOADED_SUFFIXES)
	{
		for (int i = 0; i < MAX_BONUS; i++)
		{
			SUFFIXES[i] = I18n::Translate("armor-suffix-" + to_string(i));
		}
		LOADED_SUFFIXES = true;
	}

	if (ArmorHandler::ARMORS.size() == 0)
	{
		ArmorHandler::LoadArmors();
	}

	_bonus = rand() % MAX_BONUS;
	_value = def + _bonus;
	_description = "+" + to_string(_bonus) + " " + // Show the bonus on the weapon.
				   _name +						   // Show the weapon's name
				   " " + SUFFIXES[_bonus] +        // Each bonus has a suffix.
				   " at " + to_string(_value);	 // Show total value too
}

// The default armor piece.
CArmor::CArmor() {
	if (!LOADED_SUFFIXES)
	{
		for (int i = 0; i < MAX_BONUS; i++)
		{
			SUFFIXES[i] = I18n::Translate("armor-suffix-" + to_string(i));
		}
		LOADED_SUFFIXES = true;
	}

	if (ArmorHandler::ARMORS.size() == 0)
	{
		ArmorHandler::LoadArmors();
	}

	*this = CArmor(ArmorHandler::ARMORS[0].name, ArmorHandler::ARMORS[0].defense);
}

bool CArmor::use()
{
	cout << I18n::Translate("armor-use") << endl;
	return false;
}

const string ArmorHandler::FILE_NAME = "Armors.yaml";

vector<ArmorData> ArmorHandler::ARMORS = {};

void ArmorHandler::LoadArmors()
{
	// Load the file up.
	bool err;
	YamlContainer armorData = LoadYamlFromFile(FILE_NAME, &err);

	if (err)
	{
		cerr << "[CRITICAL] Invalid armor data " << FILE_NAME << ". Armor will be permanently broken!" << endl;
	}

	// Get the armor list item. We dereference so that we can properly use the indexer.
	YamlList armorList = *(static_cast<YamlList*> (armorData["armor-list"]));

	// Add all of the armors.
	for (int i = 0; i < armorList.itemCount(); i++)
	{
		YamlContainer armorItem = *(static_cast<YamlContainer*>(armorList[i]));

		// Get the name.
		string name = armorItem["name"]->value();

		// Parse out the value.
		string value_str = armorItem["value"]->value();

		char* err;
		int value = strtol(value_str.c_str(), &err, 0);

		if (*err != '\0') {
			value = -1;
		}

		// Now add the armor.
		ARMORS.push_back({ name, value });
	}
}