#include "ParlessGameYLAD.h"
#include "Utils/MemoryMgr.h"
#include "Utils/Trampoline.h"
#include "Utils/Patterns.h"
#include <MinHook.h>

using namespace hook;
using namespace Memory;

ParlessGameYLAD::t_orgYLaDAddFileEntry ParlessGameYLAD::orgYLaDAddFileEntry = NULL;
ParlessGameYLAD::t_orgYLaDAddFileEntry* ParlessGameYLAD::hookYLaDAddFileEntry = NULL;

ParlessGameYLAD::t_orgYLaDFilepath ParlessGameYLAD::orgYLaDFilepath = NULL;
ParlessGameYLAD::t_orgYLaDFilepath* ParlessGameYLAD::hookYLaDFilepath = NULL;

bool ParlessGameYLAD::hook_add_file()
{
	void* renameFilePathsFunc;

	// Y7 need hooks for two different functions

// File loading hook
	if (isUwp)
	{
		renameFilePathsFunc = get_pattern("48 83 EC 28 4C 8B C2 4C 8D 4C 24 40 BA 10 04 00 00 E8", 17);
	}
	else
	{
		auto pat = pattern("48 89 54 24 10 4C 89 44 24 18 4C 89 4C 24 20 48 83 EC 28 4C 8B C2 4C 8D 4C 24 40 BA 10 04 00 00 E8");

		// Try the UWP pattern if the main pattern doesn't match
		if (pat.size() < 5)
		{
			renameFilePathsFunc = get_pattern("48 83 EC 28 4C 8B C2 4C 8D 4C 24 40 BA 10 04 00 00 E8", 17);
		}
		else
		{
			renameFilePathsFunc = pat.get(4).get<void>(32);
		}
	}

	ReadCall(renameFilePathsFunc, hookYLaDAddFileEntry);

	if (MH_CreateHook(hookYLaDAddFileEntry, &ParlessGameYLAD::YLaDAddFileEntry, reinterpret_cast<LPVOID*>(&orgYLaDAddFileEntry)) != MH_OK)
		return false;

	if (MH_EnableHook(hookYLaDAddFileEntry) != MH_OK)
		return false;

	// Filepath hook
	if (isUwp)
	{
		renameFilePathsFunc = get_pattern("44 8D 4F 03 BA 10 04 00 00 4C 8D 84 24 50 06 00 00 48 8D 8C 24 60 0A 00 00 E8", 25);
	}
	else
	{
		renameFilePathsFunc = get_pattern("44 8D 4D 03 4C 8D 84 24 40 02 00 00 BA 10 04 00 00 48 8D 8C 24 B0 0C 00 00 E8", 25);
	}

	ReadCall(renameFilePathsFunc, hookYLaDFilepath);

	if (MH_CreateHook(hookYLaDFilepath, &YLaDFilepath, reinterpret_cast<LPVOID*>(&orgYLaDFilepath)) != MH_OK)
		return false;

	if (MH_EnableHook(hookYLaDFilepath) != MH_OK)
		return false;

	return true;
}