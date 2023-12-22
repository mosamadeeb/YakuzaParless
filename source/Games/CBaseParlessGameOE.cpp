#include "CBaseParlessGameOE.h"
#include "StringHelpers.h"

std::string CBaseParlessGameOE::translate_path_original(std::string path, int indexOfData)
{
	std::vector<int> parts = splitPath(path, indexOfData, 3); //TODO!!!

	string sub;
	parless_stringmap::const_iterator match;

	const int START = 1; // Start index for checking paths
	const int MAX_SPLITS = (START + 1) + 2; // Max splits is 2

	// Look for matches in the map
	for (int i = START + 1; i < MAX_SPLITS && i < parts.size(); i++)
	{
		sub = path.substr(parts[START], parts[i] - parts[START]);
		match = m_gameMap.find(sub);

		if (match != m_gameMap.end())
		{
			// translate path
			return path.replace(parts[START], parts[i] - parts[START], match->second);
		}
	}

	return path;
}