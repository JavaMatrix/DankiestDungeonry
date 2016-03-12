#include "YamlList.h"
#include "SimpleYaml.h"

using namespace std;
using namespace simpleyaml;

YamlList::YamlList(string* lines, int numLines)
{
	// The constructor is just an alias for the parse function.
	parse(lines, numLines);
}

IYamlEntity* YamlList::operator[](uint key)
{
	// Check for out-of-bounds.
	if (key < 0 || key >= _items.size()) return nullptr;
	return &(_items[key]);
}

IYamlEntity* YamlList::operator[](string key)
{
	// Parse a number from the key.
	char* lastParsedChar;
	int index = strtol(key.c_str(), &lastParsedChar, 0);

	// If the last parsed character is not a null terminator, the key was not a valid number.
	if (*lastParsedChar != '\0') {
		return nullptr;
	}

	// Otherwise, hand it up to the integer key indexer.
	return operator[](index);
}

string YamlList::key()
{
	return _key;
}

string YamlList::value()
{
	return "YAML List";
}

int YamlList::itemCount()
{
	return _items.size();
}

bool YamlList::parse(string* lines, int numLines)
{
	// We'll divide the lines up into items by looking for dashes at the beginning of a line.
	int* itemStartLines = new int[numLines];
	int numItems = 0;

	// We keep ignoring lines until there's not a comment.
	while (numLines > 0 && Trim(lines[0])[0] == '#')
	{
		// Shave off the first entry.
		lines++;
		numLines--;
	}

	// We must have at least 2 non-comment line.
	if (numLines < 2) return false;

	// We grab the first line to get our key.
	string name = lines[0];
	name = Trim(name);
	// Take off the colon at the end.
	name = name.substr(0, name.length() - 1);
	_key = name; 

	// We'll make sure we stay at the same indentation level for items.
	int indentation = CountIndentation(lines[1]);

	// We'll do a cursory pass through each line, trimming and finding item starts.
	for (int i = 0; i < numLines; i++)
	{
		int lineIndentation = CountIndentation(lines[i]);
		lines[i] = Trim(lines[i]);

		// Check for a new item.
		if (lineIndentation == indentation && Trim(lines[i])[0] == '-')
		{
			// Chop off the dash and re-trim.
			lines[i] = Trim(lines[i].substr(1));

			// Cache this location.
			itemStartLines[numItems] = i;
			numItems++;
		}
	}
	
	// Now we can make our containers.
	for (int i = 0; i < numItems; i++)
	{
		// Calculate the bounds.
		int first = itemStartLines[i];

		// Assume we're at the last item, then check if we're not and change it accordingly.
		int last = numLines;
		if (i < numItems - 1)
		{
			last = itemStartLines[i + 1];
		}

		// Now slice the array. Insert a fictitious container key at the beginning, like '0:'.
		// Note that last actually points to the next list item, but slice doesn't include the
		// end'th element.
		int numContainerLines;
		string firstLine = to_string(i) + ":";
		string* containerLines = Slice(lines, first, last, &numContainerLines, &firstLine);

		// Make the container.
		YamlContainer container;
		if (container.parse(containerLines, numContainerLines))
		{
			// Add it only if the parse was successful.
			_items.push_back(container);
		}
	}
	
	// Successfully parsed!
	return true;
}