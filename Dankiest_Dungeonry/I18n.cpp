#include "I18n.h"
#include "SimpleYaml.h"
#include <iostream>

using namespace simpleyaml;
using namespace std;

const string I18n::FILE_NAME = "I18n.yaml";
YamlContainer I18n::_yaml;

void I18n::Initialize()
{
	bool err;
	_yaml = LoadYamlFromFile(FILE_NAME, &err);
	if (err)
	{
		cerr << "[CRITICAL] I18n.yaml could not be loaded, program is likely to crash." << endl;
	}
}

string I18n::Translate(string key, bool suppress_warn)
{
	if (!_yaml.hasValues())
	{
		Initialize();
	}

	try
	{
		return _yaml[key]->value();
	}
	catch (exception e)
	{
		if (!suppress_warn)
			cerr << "[WARN]: Unknown I18n key " << key << "." << endl;

		return "ERRNO";
	}
}

string I18n::Translate(string* keys, int numKeys)
{
	// Go through the entities one key at a time.
	IYamlEntity* currentEntity = &_yaml;

	for (int i = 0; i < numKeys; i++)
	{
		currentEntity = (*currentEntity)[keys[i]];
	}

	return currentEntity->value();
}