#include <string>
#include <unordered_map>
#include <unordered_set>

#include <StringHelpers.h>

using namespace std;

typedef unordered_set<string> stringset;
typedef std::unordered_map<std::string, std::string> stringmap;

/// <summary>
/// Translates a path using a map
/// </summary>
/// <param name="pathMap"></param>
/// <param name="path"></param>
/// <param name="parts"></param>
/// <returns></returns>
string translatePath(stringmap pathMap, string path, vector<int> parts)
{
	string sub;
	stringmap::const_iterator match;

	const int MAX_SPLITS = 3;

	// Look for matches in the map
	for (int i = 0; i < MAX_SPLITS && i < parts.size(); i++)
	{
		sub = path.substr(parts[0], parts[i] - parts[0]);
		match = pathMap.find(sub);

		if (match != pathMap.end())
		{
			// translate path
			return path.replace(parts[0], parts[i] - parts[0], match->second);
		}
	}

	return path;
}

int getSplitIndexInSet(stringset loadedPars, string path, vector<int> parts)
{
	string sub;

	// Look for matches in the set
	for (int i = 1; i < parts.size(); i++)
	{
		sub = path.substr(parts[0], parts[i] - parts[0]);

		if (loadedPars.count(sub))
		{
			return i;
		}
	}

	return -1;
}

/// <summary>
/// Appends ".parless" to the path before the end index.
/// </summary>
/// <returns>the new modified string, or an empty string if the end index is invalid.</returns>
string getParlessPath(string path, int end)
{
	string overload = "";

	if (end != -1)
	{
		overload = path;
		overload.insert(end, ".parless");
	}

	return overload;
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

enum class Game
{
	Unsupported = -1,
	Yakuza0,
	YakuzaKiwami,
	YakuzaKiwami2,
	Yakuza3,
	Yakuza4,
	Yakuza5,
	Yakuza6,
	YakuzaLikeADragon
};

enum class Locale
{
	English,
	Japanese,
	Chinese,
	Korean
};

Game getGame(string name)
{
	if (name == "Yakuza3")
	{
		return Game::Yakuza3;
	}
	if (name == "Yakuza4")
	{
		return Game::Yakuza4;
	}
	if (name == "Yakuza5")
	{
		return Game::Yakuza5;
	}
	if (name == "Yakuza0")
	{
		return Game::Yakuza0;
	}
	if (name == "YakuzaKiwami")
	{
		return Game::YakuzaKiwami;
	}
	if (name == "Yakuza6")
	{
		return Game::Yakuza6;
	}
	if (name == "YakuzaKiwami2")
	{
		return Game::YakuzaKiwami2;
	}
	if (name == "YakuzaLikeADragon")
	{
		return Game::YakuzaLikeADragon;
	}

	return Game::Unsupported;
}

stringmap getGameMap(Game game, Locale locale)
{
	vector<const char*> loc1Vec{ "e", "j", "z", "k" };
	vector<const char*> loc2Vec{ "c", "j", "z", "k" };
	vector<const char*> loc3Vec{ "en", "jp", "zh", "ko" };

	string loc1 = loc1Vec[(int)locale];
	string loc2 = loc2Vec[(int)locale];
	string loc3 = loc3Vec[(int)locale];

	switch (game)
	{
		case Game::Yakuza0:
		case Game::YakuzaKiwami:
			return stringmap({
				{"/data/font" , "/data/fontpar/font"},
				{"/data/2d/sprite_" + loc2 , "/data/2dpar/sprite_" + loc2},
				{"/data/boot" , "/data/bootpar/boot"},
				{"/data/stay" , "/data/staypar/stay"},
				{"/data/sound" , "/data/soundpar/sound"},
				{"/data/battle" , "/data/battlepar/battle"},
				{"/data/reactor_w64" , "/data/reactorpar/reactor_w64"},
				{"/data/wdr_" + loc2 + "/common" , "/data/wdr_par_" + loc2 + "/common"},
				{"/data/wdr_" + loc2 , "/data/wdr_par_" + loc2 + "/wdr"},
				{"/data/light_anim" , "/data/light_anim/light_anim"},

				// Yakuza 0 specific
				{"/data/2d/ui_" + loc1 , "/data/2dpar/ui_" + loc1},
				{"/data/pause_" + loc1 , "/data/pausepar_" + loc1 + "/pause"},

				// Yakuza Kiwami specific
				{"/data/2d/ui_" + loc2 , "/data/2dpar/ui_" + loc2},
				{"/data/pause_" + loc2 , "/data/pausepar_" + loc2 + "/pause"},
			});
		case Game::Yakuza3:
		case Game::Yakuza4:
			return stringmap({
				{"/data/font", "/data/fontpar/font_hd_en"}, // This is always en
				{"/data/2d/cse" , "/data/2dpar/cse_" + loc3},
				{"/data/2d/picture" , "/data/2dpar/picture_" + loc3},
				{"/data/boot" , "/data/bootpar/boot_" + loc3},
				{"/data/pause" , "/data/pausepar/pause_" + loc3},
				{"/data/reactive_obj/object" , "/data/reactive_obj/object_hires"},
				{"/data/chase" , "/data/chasepar/chase"},
				{"/data/wdr_" + loc3 + "/common" , "/data/wdr_par_" + loc3 + "/common"},
				{"/data/wdr_" + loc3 , "/data/wdr_par_" + loc3 + "/wdr"},
			});
		case Game::Yakuza5:
			return stringmap({
				{"/data/font", "/data/fontpar/font_hd_en"}, // This is always en
				{"/data/font_qloc", "/data/fontpar/font_q_icons"},
				{"/data/2d/sprite" , "/data/2dpar/sprite_" + loc3},
				{"/data/2d/ui" , "/data/2dpar/ui_" + loc3},
				{"/data/boot" , "/data/bootpar/boot_" + loc3},
				{"/data/stay" , "/data/staypar/stay_" + loc3},
				{"/data/pause" , "/data/pausepar/pause_" + loc3},
				{"/data/reactor" , "/data/reactorpar/reactor"},
				{"/data/sound" , "/data/soundpar/sound"},
				{"/data/battle" , "/data/battlepar/battle"},
				{"/data/wdr_" + loc3 + "/common" , "/data/wdr_par_" + loc3 + "/common"},
				{"/data/wdr_" + loc3 , "/data/wdr_par_" + loc3 + "/wdr"},
			});
		case Game::Yakuza6:
		case Game::YakuzaKiwami2:
		case Game::YakuzaLikeADragon:
		case Game::Unsupported:
		default:
			// Dragon Engine games don't need any translation
			return stringmap();
	}
}