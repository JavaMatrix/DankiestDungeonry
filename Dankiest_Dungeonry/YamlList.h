// SimpleYaml provides classes and interfaces for dealing with a highly simplified
// subset of YAML (YAML Ain't Markup Language, http://yaml.org/).
// Note that though these files are provided in a separate file for clarity,
// they are indeed written by myself (Nathanael Page), specifically for this lab.

#pragma once
#include "IYamlEntity.h"
#include "YamlContainer.h"
#include <vector>

namespace simpleyaml
{
	typedef unsigned int uint;

	// The YamlList class represents an unordered YAML-style list. Note that JSON-style lists
	// are not supported by SimpleYaml.
	class YamlList :
		public IYamlEntity
	{
	private:
		std::string _key;
		std::vector<YamlContainer> _items;
	public:
		YamlList(std::string* lines, int numLines);
		YamlList() {}; // The default constructor does nothing.
		IYamlEntity* operator[](std::string key);
		IYamlEntity* operator[](uint key);
		std::string value();
		std::string key();
		int itemCount();
		bool parse(std::string* lines, int numLines);
	};
}