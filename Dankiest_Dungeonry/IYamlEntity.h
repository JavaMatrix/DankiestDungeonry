// SimpleYaml provides classes and interfaces for dealing with a highly simplified
// subset of YAML (YAML Ain't Markup Language, http://yaml.org/).
// Note that though these files are provided in a separate file for clarity,
// they are indeed written by myself (Nathanael Page), specifically for this lab.

#pragma once
#include <string>
#include <fstream>

namespace simpleyaml
{
	// IYamlEntity serves as a base interface for anything YAML within this library.
	class IYamlEntity
	{
	public:
		// All YAML entities need to be able to give a child (or nullptr) for a given key.
		virtual IYamlEntity* operator[](std::string key) = 0;

		// All YAML entities need to be able to give a string value. 
		virtual std::string value() = 0;

		// All YAML entities must provide a string for their parent to use as a key. Blank strings are not
		// acceptable, but the key needn't be user readable for container tags.
		virtual std::string key() = 0;

		// All YAML entities must be able to parse themselves from a list of strings.
		virtual bool parse(std::string* lines, int numLines) = 0;

		// Allow implicit conversion to string.
		inline operator std::string()
		{
			return value();
		}
	};
}