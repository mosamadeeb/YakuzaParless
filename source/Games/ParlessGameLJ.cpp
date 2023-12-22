#include "ParlessGameLJ.h"
#include "StringHelpers.h"
#include "Utils/MemoryMgr.h"
#include "Utils/Trampoline.h"
#include "Utils/Patterns.h"
#include <MinHook.h>
#include <iostream>

using namespace hook;
using namespace Memory;

ParlessGameLJ::t_orgLJAddFileEntry ParlessGameLJ::orgLJAddFileEntry = NULL;
ParlessGameLJ::t_orgLJAddFileEntry(*ParlessGameLJ::hookLJAddFileEntry) = NULL;

bool ParlessGameLJ::hook_add_file()
{
	hookLJAddFileEntry = (t_orgLJAddFileEntry*)pattern("41 57 48 8D A8 68 FE FF FF 48 81 EC 58 02 00 00 C5 F8 29 70 A8").get_first(-20);

	if (MH_CreateHook(hookLJAddFileEntry, &LJAddFileEntry, reinterpret_cast<LPVOID*>(&orgLJAddFileEntry)) != MH_OK)
	{
		cout << "Hook creation failed. Aborting.\n";
		return false;
	}

	if (MH_EnableHook(hookLJAddFileEntry) != MH_OK)
	{
		cout << "Hook could not be enabled. Aborting.\n";
		return false;
	}

	// For some unknown reason, the pointer here is pointing to an interrupt (0xCC) that replaced the first byte of the trampoline space
	*((char*)orgLJAddFileEntry) = 0x48;

	hook_BindCpk = (t_CriBind*)get_pattern("41 57 48 8B EC 48 83 EC 70 4C 8B 6D 58", -26);
	org_BindDir = (t_CriBind)get_pattern("41 57 48 83 EC 30 48 8B 74 24 78 33 ED", -23);

	if (MH_CreateHook(hook_BindCpk, &BindCpk, reinterpret_cast<LPVOID*>(&org_BindCpk)) != MH_OK)
	{
		std::cout << "Hook creation failed. Aborting.\n";
		return false;
	}

	if (MH_EnableHook(hook_BindCpk) != MH_OK)
	{
		std::cout << "Hook could not be enabled. Aborting.\n";
		return false;
	}

	// Same issue as above
	*((char*)org_BindCpk) = 0x48;

	std::cout << "Applied CPK directory bind hook.\n";

	return true;
}