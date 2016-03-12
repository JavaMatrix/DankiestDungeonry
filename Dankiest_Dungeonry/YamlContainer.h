// SimpleYaml provides classes and interfaces for dealing with a highly simplified
// subset of YAML (YAML Ain't Markup Language, http://yaml.org/).
// Note that though these files are provided in a separate file for clarity,
// they are indeed written by myself (Nathanael Page), specifically for this lab.

#pragma once
#include "IYamlEntity.h"
#include <map>

namespace simpleyaml
{
	// YamlContainer represents an upper-level 
	class YamlContainer : public IYamlEntity
	{
	private:
		std::map<std::string, IYamlEntity*> _children;
		std::string _key;
	public:
		YamlContainer(std::string* lines, int numLines);
		YamlContainer() {}; // The default constructor does nothing.
		IYamlEntity* operator[](std::string key);
		std::string value();
		std::string key();
		bool parse(std::string* lines, int numLines);
		bool hasValues();
	};
}