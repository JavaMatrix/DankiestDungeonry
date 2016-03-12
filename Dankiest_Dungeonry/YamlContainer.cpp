#include "YamlContainer.h"
#include "SimpleYaml.h"
#include "YamlList.h"
#include <iostream>

using namespace std;
using namespace simpleyaml;

YamlContainer::YamlContainer(string* lines, int numLines)
{
	parse(lines, numLines);
}

IYamlEntity* YamlContainer::operator[](string key)
{
	// If the key doesn't exist, throw an error.
	if (_children.count(key) == 0) throw out_of_range("No such key " + key + ".");

	return _children[key];
}

string YamlContainer::value()
{
	return "YAML Container";
}

string YamlContainer::key()
{
	return _key;
}

bool YamlContainer::parse(string* lines, int numLines)
{

	// We keep ignoring lines until there's not a comment.
	while (numLines > 0 && Trim(lines[0]).c_str()[0] == '#')
	{
		// Shave off the first entry.
		lines++;
		numLines--;
	}

	// We must have at least 1 non-comment line.
	if (numLines < 1) return false;

	// We grab the first line to get our key.
	string name = lines[0];
	name = Trim(name);
	// Take off the colon at the end.
	name = name.substr(0, name.length() - 1);
	_key = name;

	// We'll walk through every line, skipping the first one.
	for (int i = 1; i < numLines; i++)
	{
		// We'll strip down each line so that there's no whitespace.
		string trimline = Trim(lines[i]);

		// Skip empty lines and comments.
		if (trimline.empty() || trimline.c_str()[0] == '#')
		{
			// Nothing to do on comment lines.
		}
		// There are three cases on each line. Either we have a key-value pair, a list,
		// or another container nested inside of this one.
		// First, we'll check for lists and containers, which look unfortunately similar.
		else if (trimline.back() == ':')
		{
			// First, we'll figure out how many lines are in this object.
			int lastLine = i + 1;
			// We've got to check that we're not at the last line already, because if we are something
			// is wrong.
			if (i == numLines - 1) return false;

			// Now we count the indentation of the next line.
			int indent = CountIndentation(lines[i + 1]);

			// Now we keep going until a) the end of the file or b) an indentation less than indent.
			while (lastLine < numLines && (CountIndentation(lines[lastLine]) >= indent || Trim(lines[lastLine]).empty()))
			{
				lastLine++;
			}

			// Now we grab out those lines to pass down to the child object.
			// Note that lastLine is actually pointing to the next child object, but slice
			// does not include the end'th element.
			int numChildLines;
			string* childLines = Slice(lines, i, lastLine, &numChildLines);

			// Now we peek at the next line to see if this is a list or a container.
			string nextLines = Trim(lines[i + 1]);
			if (nextLines[0] == '-')
			{
				// This is a list, make a list object.
				YamlList* list = new YamlList;
				if (list->parse(childLines, numChildLines))
				{
					_children[list->key()] = list;
				}
			}
			else
			{
				// This is a sub-container, make a container object.
				YamlContainer* container = new YamlContainer;
				if (container->parse(childLines, numChildLines))
				{
					_children[container->key()] = container;
				}
			}

			// Skip the lines we just processed.
			i = lastLine - 1;
		}
		// Otherwise, we have a kvp line.
		else
		{
			YamlKeyValuePair* kvp = new YamlKeyValuePair;
			if (kvp->parse(lines + i, 1))
			{
				_children[kvp->key()] = kvp;
			}
		}
	}

	// Successfully parsed.
	return true;
}

bool YamlContainer::hasValues()
{
	return _children.size() > 0;
}