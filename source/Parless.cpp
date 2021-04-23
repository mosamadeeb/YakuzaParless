#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#include <Shlwapi.h>
#include <boost/multiprecision/cpp_int.hpp>

#pragma comment(lib, "Shlwapi.lib")

#include "Utils/MemoryMgr.h"
#include "Utils/Trampoline.h"
#include "Utils/Patterns.h"
#include <Maps.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>

static HMODULE hDLLModule;

using namespace std;
using namespace boost::multiprecision;

namespace Parless
{
    __int64 (*orgY3AddFileEntry)(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12);
    __int64 (*orgY5AddFileEntry)(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12, int a13, char a14);
    __int64 (*orgY0AddFileEntry)(__int64 a1, char* filepath, __int64 a3, int a4, __int64 a5, __int64 a6, char a7, __int64 a8, char a9, char a10, char a11, char a12, char a13);
    int (*orgY6AddFileEntry)(int* param_1, int* param_2, char* param_3);

    stringmap gameMap;
    stringmap fileModMap;

    stringset loadedPars;

    int loadedParsMaxSplits = 3;

    // Initialized from the INI
    bool loadMods;
    bool loadParless;

    // Logging streams
    ofstream modOverrides;
    ofstream parlessOverrides;
    ofstream allFilepaths;
    
    // Logging variables
    bool logMods;
    bool logParless;
    bool logAll;

    /// <summary>
    /// Renames file paths to load files from the mods directory or .parless paths instead of pars.
    /// </summary>
    /// <returns></returns>
    char* RenameFilePaths(char* filepath)
    {
        char* datapath;

        string path(filepath);

        size_t indexOfData = firstIndexOf(path, "/data/");

        if (indexOfData != -1)
        {
            vector<int> splits;

            if (loadParless)
            {
                // Remove parless if it's there
                path = removeParlessPath(path, indexOfData);
            }

            if (!gameMap.empty())
            {
                splits = splitPath(path, indexOfData, loadedParsMaxSplits);
                path = translatePath(gameMap, path, splits);
            }

            string override = "";
            bool overridden = false;

            if (loadParless)
            {
                splits = splitPath(path, indexOfData, loadedParsMaxSplits);

                // might want to change this later to "check" files as it's combining splits in order to allow .parless for files that are already loose
                int baseParIndex = getSplitIndexInSet(loadedPars, path, splits);
                if (baseParIndex != -1)
                {
                    baseParIndex = splits[baseParIndex];
                }

                if (endsWith(path, ".par") && baseParIndex == -1)
                {
                    loadedPars.insert(path.substr(splits[1], path.length() - splits[1] - 4));

                    size_t parSplitCount = count(path.begin() + splits[1], path.end(), '/');
                    if (parSplitCount > loadedParsMaxSplits)
                    {
                        loadedParsMaxSplits = parSplitCount;
                    }

                    // If /<parname>.parless/overwrite.txt exists, prevent the original par from loading.
                    if (filesystem::exists(path + "less/overwrite.txt"))
                    {
                        path.replace(splits[1], path.length() - splits[1], "/parless.par");
                    }
                }

                override = getParlessPath(path, baseParIndex);

                if (filesystem::exists(override))
                {
                    overridden = true;

                    path = override;
                    path.copy(filepath, path.length());
                    filepath[path.length()] = '\0';

                    if (logParless)
                    {
                        parlessOverrides << filepath + indexOfData << "\n";
                        parlessOverrides.flush();
                    }
                }
            }

            if (loadMods && !overridden)
            {
                // Get the path starting from /data/
                string dataPath = path.substr(indexOfData + 5);

                stringmap::const_iterator match = fileModMap.find(dataPath);

                if (match != fileModMap.end())
                {
                    // Redirect the path from /data/ to /mods/<ModName>/
                    path.erase(indexOfData, 5);
                    path.insert(indexOfData, "/mods/" + match->second);

                    if (filesystem::exists(path))
                    {
                        path.copy(filepath, path.length());
                        filepath[path.length()] = '\0';

                        if (logMods)
                        {
                            modOverrides << filepath + indexOfData << "\n";
                            modOverrides.flush();
                        }
                    }
                }
            }
        }

        if (logAll)
        {
            if (indexOfData == -1)
            {
                // Some files might be logged with the new path
                indexOfData = firstIndexOf(path, "/mods/");
                if (indexOfData == -1) indexOfData = 0;
            }

            allFilepaths << filepath + indexOfData << "\n";
            allFilepaths.flush();
        }

        return filepath;
    }

    __int64 Y3AddFileEntry(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12)
    {
        return orgY3AddFileEntry(a1, (__int64)RenameFilePaths((char *)filepath), a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
    }

    __int64 Y5AddFileEntry(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12, int a13, char a14)
    {
        return orgY5AddFileEntry(a1, (__int64)RenameFilePaths((char*)filepath), a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
    }

    __int64 Y0AddFileEntry(__int64 a1, char* filepath, __int64 a3, int a4, __int64 a5, __int64 a6, char a7, __int64 a8, char a9, char a10, char a11, char a12, char a13)
    {
        return orgY0AddFileEntry(a1, RenameFilePaths(filepath), a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
    }

    int Y6AddFileEntry(int* a1, int* a2, char* filepath)
    {
        return orgY6AddFileEntry(a1, a2, RenameFilePaths(filepath));
    }
};

void ReadModLoadOrder()
{
    using namespace Parless;

    const char* MLO_MAGIC = "DABABY";
    const char* MLO_FILE = "YakuzaParless.mlo";

    if (!filesystem::exists(MLO_FILE))
        return;

    ifstream mlo(MLO_FILE, ios::binary | ios::in);

    if (!mlo)
        return;

    char* magic = new char[7];
    mlo.read(magic, 6);
    magic[6] = '\0';

    if (strcmp(magic, MLO_MAGIC))
        return;

    char version[2];
    mlo.read(version, 2);

    if ((int)(*version) == 1)
    {
        char current[4];

        int modNameStart;
        int modCount;

        int fileNameStart;
        int fileCount;

        mlo.read(current, sizeof(current));
        modNameStart = (int)*current;

        mlo.read(current, sizeof(current));
        modCount = (int)*current;

        mlo.read(current, sizeof(current));
        fileNameStart = (int)*current;

        mlo.read(current, sizeof(current));
        fileCount = (int)*current;

        char* name;
        char sizeBuf[2];
        int size;

        mlo.seekg(modNameStart);

        vector<string> mods;
        for (int i = 0; i < modCount; i++)
        {
            mlo.read(sizeBuf, sizeof(sizeBuf));
            size = (int)*sizeBuf;

            name = new char[size];
            mlo.read(name, size);

            mods.push_back((string)name);
        }

        mlo.seekg(fileNameStart);

        char index[2];
        for (int i = 0; i < fileCount; i++)
        {
            mlo.read(index, sizeof(index));

            mlo.read(sizeBuf, sizeof(sizeBuf));
            size = (int)*sizeBuf;

            name = new char[size];
            mlo.read(name, size);

            fileModMap[(string)name] = mods[(int)*index];
        }
    }
}

void OnInitializeHook()
{
    std::unique_ptr<ScopedUnprotect::Unprotect> Protect = ScopedUnprotect::UnprotectSectionOrFullModule( GetModuleHandle( nullptr ), ".text" );

    using namespace Memory;
    using namespace hook;

    using namespace Parless;

    // Read INI variables

    // Obtain a path to the ASI
    wchar_t wcModulePath[MAX_PATH];
    GetModuleFileNameW(hDLLModule, wcModulePath, _countof(wcModulePath) - 3); // Minus max required space for extension
    PathRenameExtensionW(wcModulePath, L".ini");

    if (GetPrivateProfileIntW(L"Parless", L"TempDisabled", 0, wcModulePath))
    {
        WritePrivateProfileStringW(L"Parless", L"TempDisabled", L"0", wcModulePath);
        return;
    }

    if (GetPrivateProfileIntW(L"Parless", L"ParlessEnabled", 1, wcModulePath) == 0)
    {
        return;
    }

    // LooseFilesEnabled is set to 0 by default in the INI
    loadParless = GetPrivateProfileIntW(L"Overrides", L"LooseFilesEnabled ", 1, wcModulePath);
    loadMods = GetPrivateProfileIntW(L"Overrides", L"ModsEnabled", 1, wcModulePath);

    int localeValue = GetPrivateProfileIntW(L"Overrides", L"Locale", 0, wcModulePath);

    logMods = GetPrivateProfileIntW(L"Logs", L"LogMods", 0, wcModulePath);
    logParless = GetPrivateProfileIntW(L"Logs", L"LogParless", 0, wcModulePath);
    logAll = GetPrivateProfileIntW(L"Logs", L"LogAll", 0, wcModulePath);

    if (GetPrivateProfileIntW(L"Debug", L"ConsoleEnabled", 0, wcModulePath))
    {
        // Open debugging console
        AllocConsole();
        FILE* fDummy;
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
    }

    Trampoline* trampoline = Trampoline::MakeTrampoline(GetModuleHandle(nullptr));

    // Get the name of the current game
    wchar_t exePath[MAX_PATH + 1];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);

    wstring wstr(exePath);

    Game game = getGame(basenameBackslashNoExt(string(wstr.begin(), wstr.end())));
    Locale locale = localeValue < 4 && localeValue >= 0 ? Locale(localeValue) : Locale::English;

    // Initialize the virtual->real map
    gameMap = getGameMap(game, locale);

    // Check if /mods/ exists
    if (!filesystem::is_directory("mods"))
    {
        loadMods = false;
    }

    // Open log streams if logging is enabled, remove them otherwise
    if (logMods && loadMods) modOverrides.open("modOverrides.txt", ios::out);
    else remove("modOverrides.txt");

    if (logParless && loadParless) parlessOverrides.open("parlessOverrides.txt", ios::out);
    else remove("parlessOverrides.txt");

    if (logAll) allFilepaths.open("allFilespaths.txt", ios::out);
    else remove("allFilespaths.txt");

    if (loadParless || loadMods)
    {
        void* renameFilePathsFunc;

        switch (game)
        {
            case Game::Yakuza0:
            case Game::YakuzaKiwami:
                // might want to hook the other call of this function as well, but currently not necessary (?)
                renameFilePathsFunc = get_pattern("48 89 44 24 20 48 8B D5 48 8B 0D ? ? ? ?", 15);
                ReadCall(renameFilePathsFunc, orgY0AddFileEntry);

                // this will take care of every file that is read from disk
                InjectHook(renameFilePathsFunc, trampoline->Jump(Y0AddFileEntry));
                break;
            case Game::Yakuza3:
            case Game::Yakuza4:
                renameFilePathsFunc = get_pattern("66 89 83 44 02 00 00 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 A0 03 00 00", -5);
                ReadCall(renameFilePathsFunc, orgY3AddFileEntry);

                InjectHook(renameFilePathsFunc, trampoline->Jump(Y3AddFileEntry));
                break;
            case Game::Yakuza5:
                renameFilePathsFunc = get_pattern("48 89 4C 24 20 49 8B D7 48 8B 0D ? ? ? ?", 15);
                ReadCall(renameFilePathsFunc, orgY5AddFileEntry);

                InjectHook(renameFilePathsFunc, trampoline->Jump(Y5AddFileEntry));
                break;
            case Game::Yakuza6:
            {
                // Hook inside the method that calculates a string's length to add 0x20 bytes to the length
                // This is needed to prevent undefined behavior when the modified path is longer than the memory allocated to it
                auto stringLenAddr = get_pattern("8B CD 3B D9 77 47", -0x1C);

                Trampoline* trampolineStringLen = Trampoline::MakeTrampoline(stringLenAddr);

                const uint8_t spacePayload[] = {
                    0x48, 0x03, 0xD8, // add rbx, rax
                    0x2B, 0xDE, // sub ebx, esi
                    0x48, 0x83, 0xC3, 0x20, // add rbx, 0x20
                    0x48, 0x8B, 0x07, // mov rax, qword ptr ds:[rdi]
                    0xE9, 0x0, 0x0, 0x0, 0x0 // jmp stringLenAddr+8
                };

                std::byte* space = trampolineStringLen->RawSpace(sizeof(spacePayload));
                memcpy(space, spacePayload, sizeof(spacePayload));

                VP::WriteOffsetValue(space + 3 + 2 + 4 + 3 + 1, reinterpret_cast<intptr_t>(stringLenAddr) + 8);

                const uint8_t funcPayload[] = {
                    0x48, 0x8B, 0xDB, // mov rbx, rbx
                    0xE9, 0x0, 0x0, 0x0, 0x0 // jmp space
                };

                memcpy(stringLenAddr, funcPayload, sizeof(funcPayload));

                VP::InjectHook(reinterpret_cast<intptr_t>(stringLenAddr) + 3, space, PATCH_JUMP);

                // Hook the AddFileEntry method to get each filepath that is loaded in the game

                // This function we're hooking is called from multiple places and the result we want to
                // change will not be available after the function returns.
                // So instead, hook the first couple of instructions of the function
                renameFilePathsFunc = get_pattern("48 8D 8D 00 04 00 00 48 89 85 00 04 00 00", -0x62);
                Trampoline* trampolineDE = Trampoline::MakeTrampoline(renameFilePathsFunc);

                const uint8_t spacePayload2[] = {
                    0xE9, 0x0, 0x0, 0x0, 0x0, // jmp Y6AddFileEntry
                    0x48, 0x89, 0x4C, 0x24, 0x08, // mov qword ptr ss : [rsp + 8] , rcx
                    0x55, // push rbp
                    0x53, // push rbx
                    0x41, 0x55, // push r13
                    0xE9, 0x0, 0x0, 0x0, 0x0 // jmp renameFilePathsFunc+9
                };

                std::byte* space2 = trampolineDE->RawSpace(sizeof(spacePayload2));
                memcpy(space2, spacePayload2, sizeof(spacePayload2));

                intptr_t srcAddr = (intptr_t)(space2 + 5);
                orgY6AddFileEntry = {};
                memcpy(std::addressof(orgY6AddFileEntry), &srcAddr, sizeof(srcAddr));

                VP::InjectHook(space2, trampoline->Jump(Y6AddFileEntry));
                VP::WriteOffsetValue(space2 + 5 + 5 + 1 + 1 + 2 + 1, reinterpret_cast<intptr_t>(renameFilePathsFunc) + 9);

                // First byte of the function is not writable for some reason, so instead we make the instruction do nothing
                const uint8_t funcPayload2[] = {
                    0x48, 0x89, 0xC9, // mov rcx, rcx
                    0xE9, 0x0, 0x0, 0x0, 0x0, // jmp space2
                    0x90 // nop
                };

                memcpy(renameFilePathsFunc, funcPayload2, sizeof(funcPayload2));

                VP::InjectHook(reinterpret_cast<intptr_t>(renameFilePathsFunc) + 3, space2, PATCH_JUMP);
                break;
            }
            case Game::YakuzaKiwami2:
            {
                // Hook inside the method that calculates a string's length to add 0x20 bytes to the length
                // This is needed to prevent undefined behavior when the modified path is longer than the memory allocated to it
                auto stringLenAddr = get_pattern("8B C7 3B D8 77 4E", -0x1C);

                Trampoline* trampolineStringLen = Trampoline::MakeTrampoline(stringLenAddr);

                const uint8_t spacePayload[] = {
                    0x48, 0x8B, 0x09, // mov rcx, qword ptr ds:[rcx]
                    0x48, 0x83, 0xC3, 0x20, // add rbx, 0x20
                    0x48, 0x8B, 0xC1, // mov rax, rcx
                    0x48, 0xC1, 0xE8, 0x2C, // shr rax, 0x2C
                    0xE9, 0x0, 0x0, 0x0, 0x0 // jmp stringLenAddr+0xA
                };

                std::byte* space = trampolineStringLen->RawSpace(sizeof(spacePayload));
                memcpy(space, spacePayload, sizeof(spacePayload));

                VP::WriteOffsetValue(space + 3 + 4 + 3 + 4 + 1, reinterpret_cast<intptr_t>(stringLenAddr) + 0xA);

                const uint8_t funcPayload[] = {
                    0x48, 0x8B, 0xDB, // mov rbx, rbx
                    0x90, 0x90, // nop
                    0xE9, 0x0, 0x0, 0x0, 0x0 // jmp space2
                };

                memcpy(stringLenAddr, funcPayload, sizeof(funcPayload));

                VP::InjectHook(reinterpret_cast<intptr_t>(stringLenAddr) + 5, space, PATCH_JUMP);

                // Hook the AddFileEntry method to get each filepath that is loaded in the game

                // Unlike Y6, K2 has a good place to hook the call instead of hooking the first instruction of the function
                renameFilePathsFunc = get_pattern("4C 8D 44 24 20 48 8B D3 48 8B CF", -5);
                VP::ReadCall(renameFilePathsFunc, orgY6AddFileEntry);

                VP::InjectHook(renameFilePathsFunc, trampoline->Jump(Y6AddFileEntry));
                break;
            }
            case Game::YakuzaLikeADragon:
                break;
            case Game::Unsupported:
            default:
                break;
        }
    }

    ReadModLoadOrder();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    UNREFERENCED_PARAMETER(lpvReserved);

    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        hDLLModule = hinstDLL;
    }
    return TRUE;
}