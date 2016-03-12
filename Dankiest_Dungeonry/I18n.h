#pragma once
#include "SimpleYaml.h"
#include <string> 

// The I18n class contains functions for internationalization.
class I18n
{
	static simpleyaml::YamlContainer _yaml;
	static const std::string FILE_NAME;
public:
	static void Initialize();
	static std::string Translate(std::string key, bool suppress_warn = false);
	static std::string Translate(std::string* keys, int numKeys);
};
