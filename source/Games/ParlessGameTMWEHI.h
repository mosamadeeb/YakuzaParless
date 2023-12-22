#include "CBaseParlessGameDE.h"

class ParlessGameTMWEHI : public CBaseParlessGameDE
{
public:
	std::string get_name() override
	{
		return "Like A Dragon Gaiden: The Man Who Erased His Name";
	}

	parless_stringmap get_game_map(Locale locale) override
	{
		std::string curLoc;
		std::vector<const char*> locGaidenVec{ "de", "en", "es", "fr", "it", "ja", "ko", "zh", "zhs", "ru", "pt"};

		parless_stringmap result = parless_stringmap();
		result["/entity"] = "/entity_aston";
		result["/ui.aston/texture"] = "/ui.aston.common/texture";

		for (int i = 0; i < locGaidenVec.size(); i++)
		{
			curLoc = std::string(locGaidenVec[i]);
			result["/db.aston/" + curLoc] = "/db.aston." + curLoc;
			result["/ui.aston/" + curLoc] = "/ui.aston." + curLoc;
		}

		return result;
	}

	bool hook_add_file() override;

	typedef int (*t_orgGaidenAddFileEntry)(short* a1, int a2, char* a3, char** a4);
	static t_orgGaidenAddFileEntry orgGaidenAddFileEntry;
	static t_orgGaidenAddFileEntry(*hookGaidenAddFileEntry);

	static int GaidenAddFileEntry(short* a1, int a2, char* a3, char** a4)
	{
		orgGaidenAddFileEntry(a1, a2, a3, a4);
		RenameFilePaths((char*)a1);
		return strlen((char*)a1);
	}
};