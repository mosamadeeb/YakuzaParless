#include "CBaseParlessGame.h"

class ParlessGameOOE : public CBaseParlessGame
{
public:
	std::string get_name() override
	{
		return "Yakuza 3/4";
	};

	parless_stringmap get_game_map(Locale locale)
	{
		std::vector<const char*> loc3Vec{ "en", "ja", "zh", "ko" };
		std::string loc3 = loc3Vec[(int)locale];

		return parless_stringmap({
				{"/font", "/fontpar/font_hd_en"}, // This is always en
				{"/2d/cse" , "/2d/cse_" + loc3},
				{"/2d/picture" , "/2d/picture_" + loc3},
				{"/boot" , "/bootpar/boot_" + loc3},
				{"/pause" , "/pausepar/pause_" + loc3},
				{"/reactive_obj/object" , "/reactive_obj/object_hires"},
				{"/chase" , "/chasepar/chase"},
				{"/wdr_" + loc3 + "/common" , "/wdr_par_" + loc3 + "/common"},
				{"/wdr_" + loc3 , "/wdr_par_" + loc3 + "/wdr"},
			});
	};

	bool hook_add_file() override;

	static __int64 (*orgOOEAddFileEntry)(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12);
	static __int64 (*orgOOEAdxEntry)(__int64 a1, __int64 a2, __int64 a3);

	static __int64 OOEAddFileEntry(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12)
	{
		RenameFilePaths((char*)filepath);
		return orgOOEAddFileEntry(a1, filepath, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
	}

	static __int64 OOEAdxEntry(__int64 filepath, __int64 a2, __int64 a3)
	{
		__int64 result = orgOOEAdxEntry(filepath, a2, a3);
		RenameFilePaths((char*)filepath);
		return result;
	}
};