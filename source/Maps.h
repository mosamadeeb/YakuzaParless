#include <unordered_map>
#include <unordered_set>

#include <StringHelpers.h>

using namespace std;

typedef unordered_set<string> stringset;
typedef std::unordered_map<std::string, std::string> stringmap;

enum class Game
{
	Unsupported = -1,

	// Old Engine
	Yakuza3,
	Yakuza4,
	Yakuza5,
	Yakuza0,
	YakuzaKiwami,

	// Dragon Engine
	Yakuza6,
	YakuzaKiwami2,
	YakuzaLikeADragon,
	Judgment,
	LostJudgment,
	VFeSports,
	LikeADragonGaidenTheManWhoErasedHisName,
	LikeADragonInfiniteWealthDemo,
	LikeADragonInfiniteWealth
};

Game getGame(string name)
{

	if (startsWith(name, "Yakuza3")) return Game::Yakuza3;
	if (startsWith(name, "Yakuza4")) return Game::Yakuza4;
	if (startsWith(name, "Yakuza5")) return Game::Yakuza5;
	if (startsWith(name, "Yakuza0")) return Game::Yakuza0;
	if (name == "YakuzaKiwami") return Game::YakuzaKiwami;
	if (startsWith(name, "Yakuza6")) return Game::Yakuza6;
	if (startsWith(name, "YakuzaKiwami2")) return Game::YakuzaKiwami2;
	if (name == "YakuzaLikeADragon") return Game::YakuzaLikeADragon;
	if (name == "eve") return Game::VFeSports;
	if (name == "Judgment") return Game::Judgment;
	if (name == "LostJudgment") return Game::LostJudgment;
	if (name == "LikeaDragonGaiden") return Game::LikeADragonGaidenTheManWhoErasedHisName;
	if (name == "LikeADragonGaiden") return Game::LikeADragonGaidenTheManWhoErasedHisName;
	if (name == "LikeADragon8") return Game::LikeADragonInfiniteWealth;

	return Game::Unsupported;
}

const char* getGameName(Game game)
{
	switch (game)
	{
		case Game::Yakuza3:
			return "Yakuza 3 Remastered";
		case Game::Yakuza4:
			return "Yakuza 4 Remastered";
		case Game::Yakuza5:
			return "Yakuza 5 Remastered";
		case Game::Yakuza0:
			return "Yakuza 0";
		case Game::YakuzaKiwami:
			return "Yakuza Kiwami";
		case Game::Yakuza6:
			return "Yakuza 6: The Song of Life";
		case Game::YakuzaKiwami2:
			return "Yakuza Kiwami 2";
		case Game::YakuzaLikeADragon:
			return "Yakuza: Like a Dragon";
		case Game::Judgment:
			return "Judgment";
		case Game::LostJudgment:
			return "Lost Judgment";
		case Game::VFeSports:
			return "Virtua Fighter eSports";
		case Game::LikeADragonGaidenTheManWhoErasedHisName:
			return "Like a Dragon Gaiden: The Man Who Erased His Name";
		case Game::LikeADragonInfiniteWealthDemo:
			return "Like a Dragon: Infinite Wealth";
		case Game::LikeADragonInfiniteWealth:
			return "Like a Dragon: Infinite Wealth";
		case Game::Unsupported:
		default:
			return "Unsupported";
	}
}

string removeParlessPath(string path, int indexOfData)
{
	size_t pos = firstIndexOf(path, ".parless", indexOfData);

	if (pos != -1)
	{
		path = path.erase(pos, 8);
	}

	return path;
}

string removeModPath(string path, int indexOfData)
{
	size_t pos = firstIndexOf(path, "/", indexOfData + 6);

	if (pos != -1)
	{
		path = path.replace(indexOfData, pos - indexOfData, "data");
	}

	return path;
}
/*
stringmap getGameMap(Game game, Locale locale)
{
	vector<const char*> loc1Vec{ "e", "j", "z", "k" };
	vector<const char*> loc2Vec{ "c", "j", "z", "k" };
	vector<const char*> loc3Vec{ "en", "ja", "zh", "ko" };

	string loc1 = loc1Vec[(int)locale];
	string loc2 = loc2Vec[(int)locale];
	string loc3 = loc3Vec[(int)locale];

	stringmap result;

	switch (game)
	{
		case Game::Yakuza0:
		case Game::YakuzaKiwami:
			result = stringmap({
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

			if (game == Game::Yakuza0)
			{
				// Yakuza 0 specific
				result["/2d/ui_" + loc1] = "/2dpar/ui_" + loc1;
				result["/pause_" + loc1] = "/pausepar_" + loc1 + "/pause";
			}
			else
			{
				// Yakuza Kiwami specific
				result["/2d/ui_" + loc2] = "/2dpar/ui_" + loc2;
				result["/pause_" + loc2] = "/pausepar/pause_" + loc2;
			}

			return result;
		case Game::Yakuza3:
		case Game::Yakuza4:
			return stringmap({
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
		case Game::Yakuza5:
			return stringmap({
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
		case Game::Yakuza6:
		case Game::YakuzaKiwami2:
			// Most Dragon Engine games don't need any major path translation
			return stringmap();
		case Game::YakuzaLikeADragon:
		{
			string curLoc;
			vector<const char*> locY7Vec{ "de", "en", "es", "fr", "it", "ja", "ko", "pt", "ru", "zh", "zhs" };

			result = stringmap();
			result["/entity"] = "/entity_yazawa";

			for (int i = 0; i < locY7Vec.size(); i++)
			{
				curLoc = string(locY7Vec[i]);
				result["/db.yazawa/" + curLoc] = "/db.yazawa." + curLoc + "/" + curLoc;
				result["/ui.yazawa/" + curLoc] = "/ui.yazawa." + curLoc + "/" + curLoc;
			}

			return result;
		}
		case Game::Judgment:
		{
			string curLoc;
			vector<const char*> locJudgeVec{ "de", "en", "es", "fr", "it", "ja", "ko", "zh", "zhs" };

			result = stringmap();
			result["/entity"] = "/entity_judge";

			for (int i = 0; i < locJudgeVec.size(); i++)
			{
				curLoc = string(locJudgeVec[i]);
				result["/db.judge/" + curLoc] = "/db.judge." + curLoc + "/" + curLoc;
				result["/ui.judge/" + curLoc] = "/ui.judge." + curLoc + "/" + curLoc;
			}

			return result;
		}
		case Game::LostJudgment:
		{
			string curLoc;
			vector<const char*> locJudgeVec{ "de", "en", "es", "fr", "it", "ja", "ko", "zh", "zhs" };

			result = stringmap();
			result["/entity"] = "/entity_coyote";
			result["/ui.coyote/texture"] = "/ui.coyote.common/texture";

			for (int i = 0; i < locJudgeVec.size(); i++)
			{
				curLoc = string(locJudgeVec[i]);
				result["/db.coyote/" + curLoc] = "/db.coyote." + curLoc;
				result["/ui.coyote/" + curLoc] = "/ui.coyote." + curLoc;
			}

			return result;
		}
		case Game::LikeADragonGaidenTheManWhoErasedHisName:
		{
			string curLoc;
			vector<const char*> locGaidenVec{ "de", "en", "es", "fr", "it", "ja", "ko", "zh", "zhs", "ru"};

			result = stringmap();
			result["/entity"] = "/entity_aston";
			result["/ui.aston/texture"] = "/ui.aston.common/texture";

			for (int i = 0; i < locGaidenVec.size(); i++)
			{
				curLoc = string(locGaidenVec[i]);
				result["/db.aston/" + curLoc] = "/db.aston." + curLoc;
				result["/ui.aston/" + curLoc] = "/ui.aston." + curLoc;
			}

			return result;
		}

		case Game::LikeADragonInfiniteWealthDemo:
		{
			string curLoc;
			vector<const char*> locInfWealthVec{ "de", "en", "es", "fr", "it", "ja", "ko", "zh", "zhs", "ru" };

			result = stringmap();
			result["/entity"] = "/entity_elvis";
			result["/ui.elvis/texture"] = "/ui.elvis.common/texture";

			for (int i = 0; i < locInfWealthVec.size(); i++)
			{
				curLoc = string(locInfWealthVec[i]);
				result["/db.elvis.trial/" + curLoc] = "/db.elvis.trial" + curLoc;
				result["/ui.elvis/" + curLoc] = "/ui.elvis." + curLoc;
			}

			return result;
		}

		case Game::LikeADragonInfiniteWealth:
		{
			string curLoc;
			vector<const char*> locInfWealthVec{ "de", "en", "es", "fr", "it", "ja", "ko", "zh", "zhs", "ru" };

			result = stringmap();
			result["/entity"] = "/entity_elvis";
			result["/ui.elvis/texture"] = "/ui.elvis.common/texture";

			for (int i = 0; i < locInfWealthVec.size(); i++)
			{
				curLoc = string(locInfWealthVec[i]);
				result["/db.elvis/" + curLoc] = "/db.elvis." + curLoc;
				result["/ui.elvis/" + curLoc] = "/ui.elvis." + curLoc;
			}

			return result;
		}
		case Game::VFeSports:
			return stringmap({
				{"/entity", "/entity_adam"},
				{"/db.adam", "/db.adam.ja"},
				{"/ui.adam", "/ui.adam.ja"},
			});
		case Game::Unsupported:
		default:
			return stringmap();
	}
}
*/