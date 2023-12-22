#include "CBaseParlessGameDE.h"
#include <iostream>

class ParlessGameYLAD : public CBaseParlessGameDE
{
	virtual std::string get_name() override
	{
		return "Yakuza: Like a Dragon";
	};

	virtual parless_stringmap get_game_map(Locale locale) override
	{
		std::string curLoc;
		std::vector<const char*> locY7Vec{ "de", "en", "es", "fr", "it", "ja", "ko", "pt", "ru", "zh", "zhs", "pt"};

		parless_stringmap result = parless_stringmap();
		result["/entity"] = "/entity_yazawa";

		for (int i = 0; i < locY7Vec.size(); i++)
		{
			curLoc = std::string(locY7Vec[i]);
			result["/db.yazawa/" + curLoc] = "/db.yazawa." + curLoc + "/" + curLoc;
			result["/ui.yazawa/" + curLoc] = "/ui.yazawa." + curLoc + "/" + curLoc;
		}

		return result;
	}

	virtual bool hook_add_file() override;

private:
	typedef char* (*t_orgYLaDAddFileEntry)(char* a1, uint64_t a2, char* a3, char* a4);
	static t_orgYLaDAddFileEntry orgYLaDAddFileEntry;
	static t_orgYLaDAddFileEntry(*hookYLaDAddFileEntry);

	typedef void (*t_orgYLaDFilepath)(char* a1, uint64_t a2, char* a3, uint64_t a4);
	static t_orgYLaDFilepath orgYLaDFilepath;
	static t_orgYLaDFilepath(*hookYLaDFilepath);

	static char* YLaDAddFileEntry(char* a1, uint64_t a2, char* a3, char* a4)
	{
		char* result = orgYLaDAddFileEntry(a1, a2, a3, a4);
		RenameFilePaths(a1);
		return result;
	}

	static void YLaDFilepath(char* a1, uint64_t a2, char* a3, uint64_t a4)
	{
		RenameFilePaths(a3);
		orgYLaDFilepath(a1, a2, a3, a4);
	}
};