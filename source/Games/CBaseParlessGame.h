#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

typedef std::unordered_set<std::string> parless_stringset;
typedef std::unordered_map<std::string, std::string> parless_stringmap;

enum class Locale
{
	English,
	Japanese,
	Chinese,
	Korean
};

typedef bool (*t_parlessRename)(char* a1);
typedef int (*t_CriBind)(void* param_1, void* param_2, const char* path, void* param_4, int param_5, void* bindId, int param_7);
typedef void (*t_CriBindPath)(void* param_1, void* param_2, const char* filepath, int param_7);


class CBaseParlessGame
{
public:
	Locale locale;

	bool isXbox;
	bool isUwp;

	virtual std::string get_name();
	virtual bool can_rebuild_mlo();
	virtual parless_stringmap get_game_map(Locale locale);

	virtual void init();

	std::string translate_path_original(std::string path, int indexOfData);
	virtual std::string translate_path(std::string path, int indexOfData);
	virtual bool hook_add_file();
	virtual bool hook_misc();
	virtual void reload();

	static bool RenameFilePaths(char* a1) { return parless_rename_func(a1); } //TEMP!!!!
	
	static t_CriBind(*hook_BindCpk);
	static t_CriBind org_BindCpk;
	static t_CriBind org_BindDir;

	
	static int BindCpk(void* param_1, void* param_2, const char* path, void* param_4, int param_5, void* bindId, int param_7)
	{
		std::cout << std::endl;
		std::cout << "Binding CPK: " << path << std::endl;
		BindCpkPaths(param_1, param_2, path, param_7);
		return org_BindCpk(param_1, param_2, path, param_4, param_5, bindId, param_7);
	}
	static void BindCpkPaths(void* param_1, void* param_2, const char* filepath, int param_7) 
	{ 
		parless_cpk_bind_path_func(param_1, param_2, filepath, param_7); 
	}

	static t_parlessRename parless_rename_func;
	static t_CriBindPath parless_cpk_bind_path_func;

protected:
	parless_stringmap m_gameMap;
};