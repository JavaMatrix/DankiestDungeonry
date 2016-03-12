#include <string>
#include <sstream>
#include "SimpleYaml.h"

using namespace std;
using namespace simpleyaml;

inline void Untrash(string& str)
{
	// Lowerize the string
	for (uint i = 0; i < str.length(); i++) str[i] = tolower(str[i]);

	// Get rid of useless words
	stringstream ss;
	string token;
	char* rest_of_str = const_cast<char*>(str.c_str());
	char* chrtok = strtok_s(rest_of_str, " ", &rest_of_str);
	if (chrtok == nullptr) return;
	token = string(chrtok);
	while (!token.empty())
	{
		// Add back in only unuseless tokens.
		if (token != "to" && token != "the" && token != "a" && token != "some")
		{
			ss << token << " ";
		}
		chrtok = strtok_s(nullptr, " ", &rest_of_str);
		if (chrtok != nullptr)
			token = string(chrtok);
		else
			token = "";
	}

	// Trim the string.
	str = Trim(ss.str());
}