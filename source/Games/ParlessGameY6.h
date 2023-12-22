#include "CBaseParlessGameDE.h"

class ParlessGameY6 : public CBaseParlessGameDE
{
	static int (*orgY6AddFileEntry)(int* param_1, int* param_2, char* param_3);
	static uint64_t(*orgY6SprintfAwb)(uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4);

	virtual std::string get_name() override
	{
		return "Yakuza 6: The Song Of Life";
	};

	virtual parless_stringmap get_game_map(Locale locale) override
	{
		return parless_stringmap();
	};

	virtual std::string translate_path(std::string path, int indexOfData) override;
	virtual bool hook_add_file() override;

	static int Y6AddFileEntry(int* a1, int* a2, char* filepath)
	{
		RenameFilePaths(filepath);
		return orgY6AddFileEntry(a1, a2, filepath);
	}

	static uint64_t Y6SprintfAwb(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4)
	{
		uint64_t result = orgY6SprintfAwb(a1, a2, a3, a4);
		RenameFilePaths((char*)result);
		return result;
	}
};