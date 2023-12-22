#include "CBaseParlessGameOE.h"

class ParlessGameY0 : public CBaseParlessGameOE
{
	static __int64 (*orgY0AddFileEntry)(__int64 a1, char* filepath, __int64 a3, int a4, __int64 a5, __int64 a6, char a7, __int64 a8, char a9, char a10, char a11, char a12, char a13);
	static __int64 (*orgY0CpkEntry)(__int64 a1, __int64 a2, __int64 a3, __int64 a4);
public:
	std::string get_name() override;

	virtual parless_stringmap get_game_map(Locale locale) override
	{
		std::vector<const char*> loc2Vec{ "c", "j", "z", "k" };
		std::string loc2 = loc2Vec[(int)locale];

		return 	parless_stringmap({
				{"/font" , "/fontpar/font"},
				{"/2d/sprite_" + loc2 , "/2dpar/sprite_" + loc2},
				{"/boot" , "/bootpar/boot"},
				{"/stay" , "/staypar/stay"},
				{"/sound" , "/soundpar/sound"},
				{"/battle" , "/battlepar/battle"},
				{"/reactor_w64" , "/reactorpar/reactor_w64"},
				{"/wdr_" + loc2 + "/common" , "/wdr_par_" + loc2 + "/common"},
				{"/wdr_" + loc2 , "/wdr_par_" + loc2 + "/wdr"},
				{"/light_anim" , "/light_anim/light_anim"},
			});
	};

	bool hook_add_file() override;


	static __int64 Y0AddFileEntry(__int64 a1, char* filepath, __int64 a3, int a4, __int64 a5, __int64 a6, char a7, __int64 a8, char a9, char a10, char a11, char a12, char a13)
	{
		RenameFilePaths(filepath);
		return orgY0AddFileEntry(a1, filepath, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
	}

	static __int64 Y0CpkEntry(__int64 a1, __int64 filepath, __int64 a3, __int64 a4)
	{
		RenameFilePaths((char*)filepath);
		return orgY0CpkEntry(a1, filepath, a3, a4);
	}

	__int64 Y0BindCPKDir(void* param_1, void* param_2, const char* path, void* param_4, int param_5, void* bindId, int param_7)
	{
		typedef int (*func1)();
		func1 func_1 = (func1)0x140B31710;

		if (!func_1())
			return -1;
	}
};