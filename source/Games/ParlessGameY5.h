#include "CBaseParlessGameOE.h"

class ParlessGameY5 : public CBaseParlessGameOE
{
public:
	std::string get_name() override
	{
		return "Yakuza 5";
	}

	parless_stringmap get_game_map(Locale locale) override
	{
		std::vector<const char*> loc3Vec{ "en", "ja", "zh", "ko" };
		std::string loc3 = loc3Vec[(int)locale];

		return parless_stringmap({
				{"/font", "/fontpar/font_hd_en"}, // This is always en
				{"/font_qloc", "/fontpar/font_q_icons"},
				{"/2d/sprite" , "/2dpar/sprite_" + loc3},
				{"/2d/ui" , "/2dpar/ui_" + loc3},
				{"/boot" , "/bootpar/boot_" + loc3},
				{"/stay" , "/staypar/stay_" + loc3},
				{"/pause" , "/pausepar/pause_" + loc3},
				{"/reactor" , "/reactorpar/reactor"},
				{"/sound" , "/soundpar/sound"},
				{"/battle" , "/battlepar/battle"},
				{"/wdr_" + loc3 + "/common" , "/wdr_par_" + loc3 + "/common"},
				{"/wdr_" + loc3 , "/wdr_par_" + loc3 + "/wdr"},
			});
	};

	bool hook_add_file() override;


public:
	static __int64 (*orgY5AddFileEntry)(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12, int a13, char a14);
	typedef char* (*t_Y5AddStreamFile)(__int64 unknown, char* filePath);

	static t_Y5AddStreamFile hook_Y5AddStreamFile;
	static t_Y5AddStreamFile org_Y5AddStreamFile;

	static __int64 Y5AddFileEntry(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12, int a13, char a14)
	{
		RenameFilePaths((char*)filepath);
		return orgY5AddFileEntry(a1, filepath, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
	}

	static void Y5AddStreamFile(__int64 unknown, char* filePath)
	{
		RenameFilePaths(filePath);
		org_Y5AddStreamFile(unknown, filePath);
	}
};