#include "ParlessGameYK1.h"
#include "StringHelpers.h"
#include "Utils/MemoryMgr.h"
#include "Utils/Trampoline.h"
#include "Utils/Patterns.h"
#include <MinHook.h>
#include <iostream>

using namespace hook;
using namespace Memory;

__int64 (*ParlessGameYK1::orgYK1AddFileEntry)(__int64 a1, char* filepath, __int64 a3, int a4, __int64 a5, __int64 a6, char a7, __int64 a8, char a9, char a10, char a11, char a12, char a13) = NULL;
__int64 (*ParlessGameYK1::orgYK1CpkEntry)(__int64 a1, __int64 a2, __int64 a3, __int64 a4) = NULL;

std::string ParlessGameYK1::get_name()
{
	return "Yakuza Kiwami";
}

bool ParlessGameYK1::hook_add_file()
{
	void* renameFilePathsFunc;

	Trampoline* trampoline = Trampoline::MakeTrampoline(GetModuleHandle(nullptr));

	hook_BindCpk = (t_CriBind*)get_pattern("41 57 48 8B EC 48 83 EC 70 4C 8B 6D 58", -26);
	org_BindDir = (t_CriBind)get_pattern("41 57 48 8B EC 48 83 EC ? 4C 8B 6D 58", -26);

	/*
	if (MH_CreateHook(hook_BindCpk, &CBaseParlessGame::BindCpk, reinterpret_cast<LPVOID*>(&org_BindCpk)) != MH_OK)
	{
		return false;
	}

	if (MH_EnableHook(hook_BindCpk) != MH_OK)
	{
		cout << "Hook could not be enabled. Aborting.\n";
		return false;
	}
	*/

	// might want to hook the other call of this function as well, but currently not necessary (?)
	renameFilePathsFunc = get_pattern("48 89 44 24 20 48 8B D5 48 8B 0D ? ? ? ?", 15);
	ReadCall(renameFilePathsFunc, orgYK1AddFileEntry);

	// this will take care of every file that is read from disk
	InjectHook(renameFilePathsFunc, trampoline->Jump(YK1AddFileEntry));

	renameFilePathsFunc = get_pattern("8B 4D DF 48 8D 70 20 49 03 F6 48 83 E6 E0", -13);
	ReadCall(renameFilePathsFunc, orgYK1CpkEntry);

	InjectHook(renameFilePathsFunc, trampoline->Jump(YK1CpkEntry));

	cout << "Applied CPK loading hook.\n";

	org_BindCpk = (t_CriBind)((char*)org_BindCpk + 1);

	return true;
}