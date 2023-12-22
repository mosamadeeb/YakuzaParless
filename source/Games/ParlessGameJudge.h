#include "CBaseParlessGameDE.h"

class ParlessGameJudge : public CBaseParlessGameDE
{
public:
	std::string get_name() override
	{
		return "Judgment";
	}

	parless_stringmap get_game_map(Locale locale) override
	{
		std::string curLoc;
		std::vector<const char*> locJudgeVec{ "de", "en", "es", "fr", "it", "ja", "ko", "zh", "zhs" };

		parless_stringmap result = parless_stringmap();
		result["/entity"] = "/entity_judge";

		for (int i = 0; i < locJudgeVec.size(); i++)
		{
			curLoc = std::string(locJudgeVec[i]);
			result["/db.judge/" + curLoc] = "/db.judge." + curLoc + "/" + curLoc;
			result["/ui.judge/" + curLoc] = "/ui.judge." + curLoc + "/" + curLoc;
		}

		return result;
	}

	bool hook_add_file() override;

	typedef int (*t_orgLJAddFileEntry)(short* a1, int a2, char* a3, char** a4);
	static t_orgLJAddFileEntry orgLJAddFileEntry;
	static t_orgLJAddFileEntry(*hookLJAddFileEntry);

	static int LJAddFileEntry(short* a1, int a2, char* a3, char** a4)
	{
		orgLJAddFileEntry(a1, a2, a3, a4);
		RenameFilePaths((char*)a1);
		return strlen((char*)a1);
	}
};