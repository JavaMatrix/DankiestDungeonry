#include "YamlKeyValuePair.h"
#include "SimpleYaml.h"

using namespace std;
using namespace simpleyaml;

YamlKeyValuePair::YamlKeyValuePair(string* lines, int numLines)
{
	// This constructor is simply an alias for the parse function.
	parse(lines, numLines);
}

IYamlEntity* YamlKeyValuePair::operator[] (string key)
{
	// KVPs never have child elements.
	return nullptr;
}

string YamlKeyValuePair::key()
{
	return _key;
}

string YamlKeyValuePair::value()
{
	return _value;
}

bool YamlKeyValuePair::parse(string* lines, int numLines)
{
	// We're going to assume just one line here, as more wouldn't make sense.
	if (numLines > 1) return false;

	// Get the line as a char array.
	string line = lines[0];
	char* cline = new char[line.length() + 1];
	strcpy_s(cline, line.length() + 1, line.c_str());

	// We'll split around the colon.
	// The strtok function hands us back strings one by one.
	char* key;
	// Needed for strtok_s, but we won't use it.
	char* rest_of_string;
	key = strtok_s(cline, ":", &rest_of_string);
	// Can't deal with null keys.
	if (key == NULL) return false;
	_key = Trim(key);

	// Using NULL lets us use the same string again and remember our place.
	char* val;
	val = strtok_s(NULL, ":", &rest_of_string);
	// Can't deal with null values.
	if (val == NULL) return false;
	_value = Trim(val);

	// Allow for rudimentary escaping
	// \c = :
	int idx = _value.find("\\c");
	while (idx != string::npos)
	{
		_value = _value.replace(idx, 2, ":");
		idx = _value.find("\\c");
	}

	// \t = tab
	idx = _value.find("\\t");
	while (idx != string::npos)
	{
		_value = _value.replace(idx, 2, "\t");
		idx = _value.find("\\t");
	}

	// \n = newline
	idx = _value.find("\\n");
	while (idx != string::npos)
	{
		_value = _value.replace(idx, 2, "\n");
		idx = _value.find("\\n");
	}

	// \s = non-breaking space
	idx = _value.find("\\s");
	while (idx != string::npos)
	{
		_value = _value.replace(idx, 2, " ");
		idx = _value.find("\\s");
	}

	// \% = % for beginning of string
	idx = _value.find("\\%");
	while (idx != string::npos)
	{
		_value = _value.replace(idx, 2, "%");
		idx = _value.find("\\%");
	}

	// \* = * for beginning of string
	idx = _value.find("\\*");
	while (idx != string::npos)
	{
		_value = _value.replace(idx, 2, "*");
		idx = _value.find("\\*");
	}

	// Successfully parsed!
	return true;
}