#include "CBaseParlessGameDE.h"

class ParlessGameIW : public CBaseParlessGameDE
{
public:
	std::string get_name() override
	{
		return "Like A Dragon: Infinite Wealth";
	}

	parless_stringmap get_game_map(Locale locale) override
	{
		std::string curLoc;
		std::vector<const char*> locInfWealthVec{ "de", "en", "es", "fr", "it", "ja", "ko", "zh", "zhs", "ru", "pt"};

		parless_stringmap result = parless_stringmap();
		result["/entity"] = "/entity_elvis";
		result["/ui.elvis/texture"] = "/ui.elvis.common/texture";

		for (int i = 0; i < locInfWealthVec.size(); i++)
		{
			curLoc = std::string(locInfWealthVec[i]);
			result["/db.elvis/" + curLoc] = "/db.elvis." + curLoc;
			result["/ui.elvis/" + curLoc] = "/ui.elvis." + curLoc;
		}

		return result;
	}

	bool hook_add_file() override;

	typedef int (*t_orgIWAddFileEntry)(short* a1, int a2, char* a3, char** a4);
	static t_orgIWAddFileEntry orgIWAddFileEntry;
	static t_orgIWAddFileEntry(*hookIWAddFileEntry);

	static int IWAddFileEntry(short* a1, int a2, char* a3, char** a4)
	{
		orgIWAddFileEntry(a1, a2, a3, a4);
		RenameFilePaths((char*)a1);
		return strlen((char*)a1);
	}
};