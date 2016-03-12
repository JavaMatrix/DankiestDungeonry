// SimpleYaml provides classes and interfaces for dealing with a highly simplified
// subset of YAML (YAML Ain't Markup Language, http://yaml.org/).
// Note that though these files are provided in a separate file for clarity,
// they are indeed written by myself (Nathanael Page), specifically for this lab.

#pragma once
#include "IYamlEntity.h"

namespace simpleyaml
{
	// YamlKeyValuePair represents a YAML-style key-value pair, like 'name: "Ken"'.
	class YamlKeyValuePair :
		public IYamlEntity
	{
	private:
		std::string _key;
		std::string _value;
	public:
		YamlKeyValuePair(std::string* lines, int numLines);
		YamlKeyValuePair() {}; // The default constructor does nothing.
		IYamlEntity* operator[](std::string key);
		std::string value();
		std::string key();
		bool parse(std::string* lines, int numLines);
	};
}