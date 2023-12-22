#include "ParlessGameY5.h"
#include "StringHelpers.h"
#include "Utils/MemoryMgr.h"
#include "Utils/Trampoline.h"
#include "Utils/Patterns.h"
#include <MinHook.h>
#include <iostream>

using namespace hook;
using namespace Memory;

__int64 (*ParlessGameY5::orgY5AddFileEntry)(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12, int a13, char a14);

char* (*ParlessGameY5::hook_Y5AddStreamFile)(__int64 unknown, char* filePath) = NULL;
char* (*ParlessGameY5::org_Y5AddStreamFile)(__int64 unknown, char* filePath) = NULL;

bool ParlessGameY5::hook_add_file()
{
	void* renameFilePathsFunc;

	Trampoline* trampoline = Trampoline::MakeTrampoline(GetModuleHandle(nullptr));

    renameFilePathsFunc = get_pattern("48 89 4C 24 20 49 8B D7 48 8B 0D ? ? ? ?", 15);
    ReadCall(renameFilePathsFunc, orgY5AddFileEntry);

    InjectHook(renameFilePathsFunc, trampoline->Jump(Y5AddFileEntry));

    hook_BindCpk = (t_CriBind*)get_pattern("41 57 48 8B EC 48 83 EC 70 4C 8B 6D 58 33 DB", -26);
    org_BindDir = (t_CriBind)get_pattern("41 57 48 83 EC 30 48 8B 74 24 78 33 ED", -23);

    if (MH_CreateHook(hook_BindCpk, &BindCpk, reinterpret_cast<LPVOID*>(&org_BindCpk)) != MH_OK)
    {
        cout << "Hook creation failed. Aborting.\n";
        return false;
    }

    if (MH_EnableHook(hook_BindCpk) != MH_OK)
    {
        cout << "Hook could not be enabled. Aborting.\n";
        return false;
    }

    org_BindCpk = (t_CriBind)((char*)org_BindCpk + 1);

    hook_Y5AddStreamFile = (t_Y5AddStreamFile)get_pattern("40 53 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 80 06 00 00");

    if (MH_CreateHook(hook_Y5AddStreamFile, &ParlessGameY5::Y5AddStreamFile, reinterpret_cast<LPVOID*>(&org_Y5AddStreamFile)) != MH_OK)
    {
        return false;
    }

    if (MH_EnableHook(hook_Y5AddStreamFile) != MH_OK)
    {
        cout << "Hook could not be enabled. Aborting.\n";
        return false;
    }

}