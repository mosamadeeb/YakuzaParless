#include "CBaseParlessGame.h"
#include "StringHelpers.h"

t_parlessRename CBaseParlessGame::parless_rename_func = NULL;
t_CriBindPath CBaseParlessGame::parless_cpk_bind_path_func = NULL;
t_CriBind CBaseParlessGame::org_BindCpk = NULL;
t_CriBind CBaseParlessGame::org_BindDir = NULL;
t_CriBind(*CBaseParlessGame::hook_BindCpk) = NULL;

void CBaseParlessGame::init()
{
	m_gameMap = get_game_map(locale);
}

std::string CBaseParlessGame::get_name()
{
	return std::string("Unknown");
}

bool CBaseParlessGame::can_rebuild_mlo()
{
	return false;
}

void CBaseParlessGame::reload()
{

}

parless_stringmap CBaseParlessGame::get_game_map(Locale locale)
{
	return parless_stringmap();
}

std::string CBaseParlessGame::translate_path_original(std::string path, int sizeOfPath)
{
	std::vector<int> parts = splitPath(path, sizeOfPath, 3); //TODO!!!

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

std::string CBaseParlessGame::translate_path(std::string path, int sizeOfPath)
{
	return translate_path_original(path, sizeOfPath);
}

bool CBaseParlessGame::hook_add_file()
{
	return false;
}


bool CBaseParlessGame::hook_misc()
{
	return false;
}