// SimpleYaml provides classes and interfaces for dealing with a highly simplified
// subset of YAML (YAML Ain't Markup Language, http://yaml.org/).
// Note that though these files are provided in a separate file for clarity,
// they are indeed written by myself (Nathanael Page), specifically for this lab.

// YamlParseUtils.h contains several useful functions, mostly operations on std::strings, to help with parsing.

#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "YamlContainer.h"
#include "YamlList.h"
#include "YamlKeyValuePair.h"
#include "IYamlEntity.h"

namespace simpleyaml
{
	// Just makes things look nicer.
	typedef unsigned int uint;

	// Trim
	// Trims all whitespace from either side of a string.
	// Usage: trim("   test string	 \n", true, true)
	inline std::string Trim(std::string str, bool left = true, bool right = true)
	{
		if (left)
		{
			uint firstNonWhite = 0;

			// Find the first index that's not whitespace.
			while (isspace(str[firstNonWhite]) && firstNonWhite < str.length())
			{
				firstNonWhite++;
			}

			// Take the substring from that position onward.
			str = str.substr(firstNonWhite);
		}

		if (right && str.length() > 0)
		{
			int lastNonWhite = str.length() - 1;

			// Find the first index that's not whitespace.
			while (isspace(str[lastNonWhite]) && lastNonWhite > 0)
			{
				lastNonWhite--;
			}

			// Take the substring up to that position.
			str = str.substr(0, lastNonWhite + 1);
		}

		return str;
	}


	// CountIndentation
	// Counts the level of indentation of a string.
	// Usage: int indent = countIndentation("  bla:");
	inline int CountIndentation(std::string str)
	{
		int count = 0;
		
		// Keep going through until we find a non-matching char. '-' counts as indentation in YAML.
		while (isspace(str[count]) || str[count] == '-')
		{
			count++;
		}

		return count;
	}

	// Slice
	// Slices out a chunk of an array and returns the chunk, optionally inserting an extra element at
	// the beginning. Count will be changed to the size of the slice.
	// Note that the slice is a copy of the original array elements.
	// Usage: slice(lines, 0, 5, count, "Item0")
	template<typename T>
	inline T* Slice(T* arr, int start, int end, int* count, T* insert = nullptr)
	{
		// Calculate the return size.
		int sz = end - start;
		if (insert != nullptr)
		{
			sz++;
		}

		// Now construct the slice.
		T* slice = new T[sz];

		// We may want to insert an element at the beginning.
		int offset = 0;
		if (insert != nullptr)
		{
			slice[0] = *insert;
			offset = 1;
		}

		for (int i = 0; i < (end - start); i++)
		{
			// Copy over the items.
			slice[i + offset] = arr[i + start];
		}

		if (count != nullptr && count != NULL)
			*count = sz;

		return slice;
	}

	// StripQuotes
	// Strips the quotes off of the front and back of a string, if they exist.
	// Good for handling YAML string literals.
	// Usage: StripQuotes("\"Hello\"");
	inline std::string StripQuotes(std::string& str)
	{
		if (str.front() == '\"')
		{
			// Chop off the first char.
			str = str.substr(1);
		}
		if (str.back() == '\"')
		{
			// Chop off the last char.
			str = str.substr(0, str.length() - 1);
		}

		return str;
	}

	// LoadYamlFromFile
	// Loads YAML from a given file and sets or clears the error flag depending on success.
	// Usage: LoadYamlFromFile("YamlMcYaml.yaml", &err)
	inline YamlContainer LoadYamlFromFile(std::string filename, bool* err)
	{
		// Open the file and make sure that it's valid.
		std::ifstream f(filename);
		if (!f.good())
		{
			*err = true;
			YamlContainer empty;
			return empty;
		}

		// Read all of the lines into an array.
		std::string line;
		std::vector<std::string> allLinesVector;
		while (std::getline(f, line))
		{
			allLinesVector.push_back(line);
		}

		std::string* linesArr = new std::string[allLinesVector.size() + 1];
		linesArr[0] = "__FILE__:"; // The fake container for the top level.

		// Copy the data from the vector to the array.
		for (uint i = 0; i < allLinesVector.size(); i++)
		{
			linesArr[i + 1] = allLinesVector[i];
		}
		
		// Parse the data into a container.
		YamlContainer dat;
		*err = !dat.parse(linesArr, allLinesVector.size() + 1);

		return dat;
	}

	inline int ToInt(std::string str)
	{
		char* lastParsedChar;
		int num = strtol(str.c_str(), &lastParsedChar, 0);

		// If the last parsed character is not a null terminator, the key was not a valid number.
		if (*lastParsedChar != '\0') {
			num = 0;
		}

		return num;
	}
}