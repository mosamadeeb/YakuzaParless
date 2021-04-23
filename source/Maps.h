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
	YakuzaLikeADragon
};

Game getGame(string name)
{
	if (name == "Yakuza3") return Game::Yakuza3;
	if (name == "Yakuza4") return Game::Yakuza4;
	if (name == "Yakuza5") return Game::Yakuza5;
	if (name == "Yakuza0") return Game::Yakuza0;
	if (name == "YakuzaKiwami") return Game::YakuzaKiwami;
	if (name == "Yakuza6") return Game::Yakuza6;
	if (name == "YakuzaKiwami2") return Game::YakuzaKiwami2;
	if (name == "YakuzaLikeADragon") return Game::YakuzaLikeADragon;

	return Game::Unsupported;
}

enum class Locale
{
	English,
	Japanese,
	Chinese,
	Korean
};

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

	const int START = 1; // Start index for checking paths
	const int MAX_SPLITS = (START + 1) + 2; // Max splits is 2

	// Look for matches in the map
	for (int i = START + 1; i < MAX_SPLITS && i < parts.size(); i++)
	{
		sub = path.substr(parts[START], parts[i] - parts[START]);
		match = pathMap.find(sub);

		if (match != pathMap.end())
		{
			// translate path
			return path.replace(parts[START], parts[i] - parts[START], match->second);
		}
	}

	return path;
}

string translatePathDE(string path, int indexOfData, Game game, Locale locale)
{
	if (firstIndexOf(path, "/data/entity", indexOfData) != -1 && endsWith(path, ".txt"))
	{
		string loc = "/ja/";
		if (locale == Locale::English)
		{
			if (game == Game::YakuzaKiwami2) loc = "/en/";
			else if (game == Game::Yakuza6) loc = "/e/";
		}

		path = rReplace(path, loc, "/");
	}

	return path;
}

int getSplitIndexInSet(stringset loadedPars, string path, vector<int> parts)
{
	string sub;

	const int START = 1; // Start index for checking paths

	// Look for matches in the set
	for (int i = START + 1; i < parts.size(); i++)
	{
		sub = path.substr(parts[START], parts[i] - parts[START]);

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

string removeModPath(string path, int indexOfData)
{
	size_t pos = firstIndexOf(path, "/", indexOfData + 7);

	if (pos != -1)
	{
		path = path.replace(indexOfData, pos - indexOfData, "/data");
	}

	return path;
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

				// Yakuza 0 specific
				{"/2d/ui_" + loc1 , "/2dpar/ui_" + loc1},
				{"/pause_" + loc1 , "/pausepar/pause_" + loc2},

				// Yakuza Kiwami specific
				{"/2d/ui_" + loc2 , "/2dpar/ui_" + loc2},
				{"/pause_" + loc2 , "/pausepar/pause_" + loc2},
			});
		case Game::Yakuza3:
		case Game::Yakuza4:
			return stringmap({
				{"/font", "/fontpar/font_hd_en"}, // This is always en
				{"/2d/cse" , "/2dpar/cse_" + loc3},
				{"/2d/picture" , "/2dpar/picture_" + loc3},
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
		case Game::YakuzaLikeADragon:
		case Game::Unsupported:
		default:
			// Dragon Engine games don't need any major path translation
			return stringmap();
	}
}