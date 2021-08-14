#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

#include "Utils/MemoryMgr.h"
#include "Utils/Trampoline.h"
#include "Utils/Patterns.h"
#include <MinHook.h>
#include <Maps.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ShellAPI.h>
#include <mutex>

static HMODULE hDLLModule;

using namespace std;

namespace Parless
{
    __int64 (*orgY3AddFileEntry)(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12);
    __int64 (*orgY5AddFileEntry)(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12, int a13, char a14);
    __int64 (*orgY0AddFileEntry)(__int64 a1, char* filepath, __int64 a3, int a4, __int64 a5, __int64 a6, char a7, __int64 a8, char a9, char a10, char a11, char a12, char a13);
    int (*orgY6AddFileEntry)(int* param_1, int* param_2, char* param_3);
    BYTE* (*orgVFeSAddFileEntry)(BYTE* a1, int a2);

    typedef uint64_t (*t_orgYLaDAddFileEntry)(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5);
    t_orgYLaDAddFileEntry orgYLaDAddFileEntry = NULL;
    t_orgYLaDAddFileEntry (*hookYLaDAddFileEntry) = NULL;

    __int64 (*orgY3AdxEntry)(__int64 a1, __int64 a2, __int64 a3);
    __int64 (*orgY0CpkEntry)(__int64 a1, __int64 a2, __int64 a3, __int64 a4);
    uint64_t(*orgY6SprintfAwb)(uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4);

    Game currentGame;
    Locale currentLocale;

    stringmap gameMap;
    stringmap fileModMap;

    unordered_map<string, int> parlessPathMap;

    int gameMapMaxSplits = 3;

    uint8_t STR_LEN_ADD = 0x40;

    // Initialized from the INI
    bool loadMods;
    bool loadParless;
    bool rebuildMLO;

    bool hasRepackedPars;

    class loggingStream {
      std::mutex m;
      std::ofstream o;
    public:
      explicit loggingStream() {};
      _Acquires_lock_(this->m) void lock() {
        this->m.lock();
      }

      _Releases_lock_(this->m) void unlock() {
        this->m.unlock();
      }

      std::ofstream& operator*() {
        return this->o;
      }

    };

    // Logging streams
    loggingStream modOverrides;
    loggingStream parlessOverrides;
    loggingStream allFilepaths;

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

        if (indexOfData == -1 && loadMods)
        {
            // File might be in mods instead, which means we're receiving it modified
            indexOfData = firstIndexOf(path, "/mods/");

            if (indexOfData != -1)
            {
                // Replace the mod path with data
                path = removeModPath(path, indexOfData);
            }
        }

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
                splits = splitPath(path, indexOfData, gameMapMaxSplits);
                path = translatePath(gameMap, path, splits);
            }

            if (currentGame >= Game::Yakuza6)
            {
                // Dragon Engine specific translation
                path = translatePathDE(path, indexOfData, currentGame, currentLocale);
            }

            string override = "";
            bool overridden = false;

            if (hasRepackedPars && endsWith(path, ".par"))
            {
                override = path;

                // Redirect the path from /data/ to /mods/Parless/
                override.erase(indexOfData, 5);
                override.insert(indexOfData, "/mods/Parless");

                if (filesystem::exists(override))
                {
                    overridden = true;

                    override.copy(filepath, override.length());
                    filepath[override.length()] = '\0';

                    if (logMods)
                    {
                        std::lock_guard<loggingStream> g_(modOverrides);
                        (*modOverrides) << filepath + indexOfData << std::endl;
                    }
                }
            }

            if (loadParless && !overridden)
            {
                int parlessIndex = -1;
                unordered_map<string, int>::const_iterator parlessPathMatch = parlessPathMap.find(pathWithoutFilename(path).substr(indexOfData + 5));

                if (parlessPathMatch != parlessPathMap.end())
                {
                    parlessIndex = parlessPathMatch->second + indexOfData + 5;

                    override = path;

                    // Check if file exists in .parless path
                    override.insert(parlessIndex, ".parless");

                    if (filesystem::exists(override))
                    {
                        overridden = true;

                        override.copy(filepath, override.length());
                        filepath[override.length()] = '\0';

                        if (logParless)
                        {
                            std::lock_guard<loggingStream> g_(parlessOverrides);
                            (*parlessOverrides) << filepath + indexOfData << std::endl;
                        }
                    }
                }
            }

            if (loadMods && !overridden)
            {
                // Get the path starting from /data/
                string dataPath = path.substr(indexOfData + 5);
                string dataPath_lowercase = dataPath;
                std::for_each(dataPath_lowercase.begin(), dataPath_lowercase.end(), [](char& w) { w = std::tolower(w); });

                stringmap::const_iterator match = fileModMap.find(dataPath_lowercase);

                if (match != fileModMap.end())
                {
                    override = path;

                    // Redirect the path from /data/ to /mods/<ModName>/
                    override.erase(indexOfData, 5);
                    override.insert(indexOfData, "/mods/" + match->second);

                    if (filesystem::exists(override))
                    {
                        overridden = true;

                        override.copy(filepath, override.length());
                        filepath[override.length()] = '\0';

                        if (logMods)
                        {
                            std::lock_guard<loggingStream> g_(modOverrides);
                            (*modOverrides) << filepath + indexOfData << std::endl;
                        }
                    }
                }
            }
        }

        if (logAll)
        {
            if (indexOfData == -1) indexOfData = 0;
            std::lock_guard<loggingStream> g_(allFilepaths);
            (*allFilepaths) << filepath + indexOfData << std::endl;
        }

        return filepath;
    }

    __int64 Y3AddFileEntry(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12)
    {
        return orgY3AddFileEntry(a1, (__int64)RenameFilePaths((char*)filepath), a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
    }

    __int64 Y3AdxEntry(__int64 filepath, __int64 a2, __int64 a3)
    {
        __int64 result = orgY3AdxEntry(filepath, a2, a3);
        filepath = (__int64)RenameFilePaths((char*)filepath);
        return result;
    }

    __int64 Y5AddFileEntry(__int64 a1, __int64 filepath, __int64 a3, int a4, __int64 a5, __int64 a6, int a7, __int64 a8, int a9, char a10, int a11, char a12, int a13, char a14)
    {
        return orgY5AddFileEntry(a1, (__int64)RenameFilePaths((char*)filepath), a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
    }

    __int64 Y0AddFileEntry(__int64 a1, char* filepath, __int64 a3, int a4, __int64 a5, __int64 a6, char a7, __int64 a8, char a9, char a10, char a11, char a12, char a13)
    {
        return orgY0AddFileEntry(a1, RenameFilePaths(filepath), a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
    }

    __int64 Y0CpkEntry(__int64 a1, __int64 filepath, __int64 a3, __int64 a4)
    {
        return orgY0CpkEntry(a1, (__int64)RenameFilePaths((char*)filepath), a3, a4);
    }

    int Y6AddFileEntry(int* a1, int* a2, char* filepath)
    {
        return orgY6AddFileEntry(a1, a2, RenameFilePaths(filepath));
    }

    uint64_t Y6SprintfAwb(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4)
    {
        return (uint64_t)RenameFilePaths((char*)orgY6SprintfAwb(a1, a2, a3, a4));
    }

    BYTE* VFeSAddFileEntry(BYTE* a1, int a2)
    {
        return (BYTE*)RenameFilePaths((char*)orgVFeSAddFileEntry(a1, a2));
    }

    uint64_t YLaDAddFileEntry(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5)
    {
        return (uint64_t)RenameFilePaths((char*)orgYLaDAddFileEntry(a1, a2, a3, a4, a5));
    }
}

void RebuildMLO()
{
    SHELLEXECUTEINFOA ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = "RyuModManagerCLI.exe";
    ShExecInfo.lpParameters = "-s";
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;
    ShellExecuteExA(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
    CloseHandle(ShExecInfo.hProcess);
}

void ReadModLoadOrder()
{
    using namespace Parless;

    const char* MLO_MAGIC = "_OLM";
    const char* MLO_FILE = "YakuzaParless.mlo";

    if (!filesystem::exists(MLO_FILE))
        return;

    ifstream mlo(MLO_FILE, ios::binary | ios::in);

    if (!mlo)
        return;

    char magic[5];
    mlo.read(magic, 4);
    magic[4] = '\0';

    if (strcmp(magic, MLO_MAGIC))
        return;

    // Skip endianness
    mlo.seekg(4, SEEK_CUR);

    uint32_t version;
    mlo.read((char*)&version, sizeof(version));

    // Skip filesize
    mlo.seekg(4, SEEK_CUR);

    if (version == 0x10002)
    {
        uint32_t modNameStart;
        uint32_t modCount;

        uint32_t fileNameStart;
        uint32_t fileCount;

        uint32_t parlessPathStart;
        uint32_t parlessPathCount;

        mlo.read((char*)&modNameStart, sizeof(modNameStart));
        mlo.read((char*)&modCount, sizeof(modCount));

        mlo.read((char*)&fileNameStart, sizeof(fileNameStart));
        mlo.read((char*)&fileCount, sizeof(fileCount));

        mlo.read((char*)&parlessPathStart, sizeof(parlessPathStart));
        mlo.read((char*)&parlessPathCount, sizeof(parlessPathCount));

        // Skip padding
        mlo.seekg(8, SEEK_CUR);

        uint16_t length;
        char* name;

        mlo.seekg(modNameStart);

        vector<string> mods;
        for (int i = 0; i < modCount; i++)
        {
            mlo.read((char*)&length, sizeof(length));

            name = new char[length];
            mlo.read(name, length);

            mods.push_back(string(name));
            delete[] name;
        }

        mlo.seekg(fileNameStart);

        uint16_t index;
        for (int i = 0; i < fileCount; i++)
        {
            mlo.read((char*)&index, sizeof(index));
            mlo.read((char*)&length, sizeof(length));

            name = new char[length];
            mlo.read(name, length);

            fileModMap[string(name)] = mods[index];
            delete[] name;
        }

        mlo.seekg(parlessPathStart);

        for (int i = 0; i < parlessPathCount; i++)
        {
            mlo.read((char*)&index, sizeof(index));
            mlo.read((char*)&length, sizeof(length));

            name = new char[length];
            mlo.read(name, length);

            parlessPathMap[string(name)] = index;
            delete[] name;
        }
    }
}

void OnInitializeHook()
{
    std::unique_ptr<ScopedUnprotect::Unprotect> Protect = ScopedUnprotect::UnprotectSectionOrFullModule(GetModuleHandle(nullptr), ".text");

    using namespace Memory::VP;
    using namespace hook;

    using namespace Parless;

    const char* VERSION = "1.3";

    const char* FILE_LOAD_MSG = "Applied file loading hook.\n";
    const char* CPK_LOAD_MSG = "Applied CPK loading hook.\n";
    const char* ADX_LOAD_MSG = "Applied ADX loading hook.\n";
    const char* AWB_LOAD_MSG = "Applied AWB loading hook.\n";
    const char* PATH_EXT_MSG = "Applied file path extension hook.\n";

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
    rebuildMLO = GetPrivateProfileIntW(L"Overrides", L"RebuildMLO", 0, wcModulePath);

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

    cout << "YakuzaParless v" << VERSION << "\n\n";

    Trampoline* trampoline = Trampoline::MakeTrampoline(GetModuleHandle(nullptr));

    // Get the name of the current game
    wchar_t exePath[MAX_PATH + 1];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);

    wstring wstr(exePath);
    string currentGameName = basenameBackslashNoExt(string(wstr.begin(), wstr.end()));

    currentGame = getGame(currentGameName);
    currentGameName = getGameName(currentGame);
    currentLocale = localeValue < 4 && localeValue >= 0 ? Locale(localeValue) : Locale::English;

    cout << "Detected game: " << currentGameName << endl;

    // Initialize the virtual->real map
    gameMap = getGameMap(currentGame, currentLocale);

    // Rebuild the MLO file
    if (rebuildMLO)
    {
        cout << "Rebuilding MLO... ";
        RebuildMLO();
        cout << "DONE!\n";
    }

    // Read MLO file
    cout << "Reading MLO... ";
    ReadModLoadOrder();
    cout << "DONE!\n";

    // These maps are filled after reading the MLO
    if (!parlessPathMap.size())
    {
        cout << "No \".parless\" paths were found in the MLO.\n";
        loadParless = false;
    }
    if (!fileModMap.size())
    {
        cout << "No mod files were found in the MLO.\n";
        loadMods = false;
    }

    // Check if repacked pars exist (old engine only)
    hasRepackedPars = filesystem::is_directory("mods/Parless");

    if (!hasRepackedPars)
    {
        cout << "No repacked pars were found.\n";
    }

    cout << endl;

    if (!(loadParless || loadMods || hasRepackedPars || logAll))
    {
        cout << "No mods were loaded. LogAll is disabled. No patches will be applied. Aborting.\n";
    }
    else
    {
        void* renameFilePathsFunc;

        // Open log streams if logging is enabled, remove them otherwise
        if (logMods) (*modOverrides).open("modOverrides.txt", ios::out);
        else remove("modOverrides.txt");

        if (logParless && loadParless) (*parlessOverrides).open("parlessOverrides.txt", ios::out);
        else remove("parlessOverrides.txt");

        if (logAll) (*allFilepaths).open("allFilespaths.txt", ios::out);
        else remove("allFilespaths.txt");

        switch (currentGame)
        {
            case Game::Yakuza0:
            case Game::YakuzaKiwami:
                // might want to hook the other call of this function as well, but currently not necessary (?)
                renameFilePathsFunc = get_pattern("48 89 44 24 20 48 8B D5 48 8B 0D ? ? ? ?", 15);
                ReadCall(renameFilePathsFunc, orgY0AddFileEntry);

                // this will take care of every file that is read from disk
                InjectHook(renameFilePathsFunc, trampoline->Jump(Y0AddFileEntry));
                cout << FILE_LOAD_MSG;

                // Cpk
                if (currentGame == Game::Yakuza0)
                {
                    renameFilePathsFunc = get_pattern("8B 4D D7 4A 8D 74 28 20 48 83 E6 E0 48 8D BE 9F 02 00 00", -13);
                    ReadCall(renameFilePathsFunc, orgY0CpkEntry);

                    InjectHook(renameFilePathsFunc, trampoline->Jump(Y0CpkEntry));
                }
                else
                {
                    // Yakuza Kiwami
                    renameFilePathsFunc = get_pattern("8B 4D DF 48 8D 70 20 49 03 F6 48 83 E6 E0", -13);
                    ReadCall(renameFilePathsFunc, orgY0CpkEntry);

                    InjectHook(renameFilePathsFunc, trampoline->Jump(Y0CpkEntry));
                }

                cout << CPK_LOAD_MSG;

                break;
            case Game::Yakuza3:
            case Game::Yakuza4:
                renameFilePathsFunc = get_pattern("66 89 83 44 02 00 00 E8 ? ? ? ? 48 8B C3 48 8B 8C 24 A0 03 00 00", -5);
                ReadCall(renameFilePathsFunc, orgY3AddFileEntry);

                InjectHook(renameFilePathsFunc, trampoline->Jump(Y3AddFileEntry));
                cout << FILE_LOAD_MSG;

                // Adx
                renameFilePathsFunc = get_pattern("48 89 5C 24 18 57 48 81 EC 90 06 00 00 48 8B 05", 0x84);
                ReadCall(renameFilePathsFunc, orgY3AdxEntry);

                InjectHook(renameFilePathsFunc, trampoline->Jump(Y3AdxEntry));
                cout << ADX_LOAD_MSG;
                break;
            case Game::Yakuza5:
                renameFilePathsFunc = get_pattern("48 89 4C 24 20 49 8B D7 48 8B 0D ? ? ? ?", 15);
                ReadCall(renameFilePathsFunc, orgY5AddFileEntry);

                InjectHook(renameFilePathsFunc, trampoline->Jump(Y5AddFileEntry));
                cout << FILE_LOAD_MSG;
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
                    0x48, 0x83, 0xC3, STR_LEN_ADD, // add rbx, 0x20
                    0x48, 0x8B, 0x07, // mov rax, qword ptr ds:[rdi]
                    0xE9, 0x0, 0x0, 0x0, 0x0 // jmp stringLenAddr+8
                };

                std::byte* space = trampolineStringLen->RawSpace(sizeof(spacePayload));
                memcpy(space, spacePayload, sizeof(spacePayload));

                WriteOffsetValue(space + 3 + 2 + 4 + 3 + 1, reinterpret_cast<intptr_t>(stringLenAddr) + 8);

                const uint8_t funcPayload[] = {
                    0x48, 0x8B, 0xDB, // mov rbx, rbx
                    0xE9, 0x0, 0x0, 0x0, 0x0 // jmp space
                };

                memcpy(stringLenAddr, funcPayload, sizeof(funcPayload));

                InjectHook(reinterpret_cast<intptr_t>(stringLenAddr) + 3, space, PATCH_JUMP);
                cout << PATH_EXT_MSG;

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

                InjectHook(space2, trampoline->Jump(Y6AddFileEntry));
                WriteOffsetValue(space2 + 5 + 5 + 1 + 1 + 2 + 1, reinterpret_cast<intptr_t>(renameFilePathsFunc) + 9);

                // First byte of the function is not writable for some reason, so instead we make the instruction do nothing
                const uint8_t funcPayload2[] = {
                    0x48, 0x89, 0xC9, // mov rcx, rcx
                    0xE9, 0x0, 0x0, 0x0, 0x0, // jmp space2
                    0x90 // nop
                };

                memcpy(renameFilePathsFunc, funcPayload2, sizeof(funcPayload2));

                InjectHook(reinterpret_cast<intptr_t>(renameFilePathsFunc) + 3, space2, PATCH_JUMP);
                cout << FILE_LOAD_MSG;

                // AWB files are not passed over to the normal file entry function
                auto sprintfAWBs = get_pattern("E8 ? ? ? ? 4C 8D 4C 24 60 45 33 C0 41 8B D7 49 8B CC");
                ReadCall(sprintfAWBs, orgY6SprintfAwb);

                InjectHook(sprintfAWBs, trampoline->Jump(Y6SprintfAwb));
                cout << AWB_LOAD_MSG;
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
                    0x48, 0x83, 0xC3, STR_LEN_ADD, // add rbx, 0x20
                    0x48, 0x8B, 0xC1, // mov rax, rcx
                    0x48, 0xC1, 0xE8, 0x2C, // shr rax, 0x2C
                    0xE9, 0x0, 0x0, 0x0, 0x0 // jmp stringLenAddr+0xA
                };

                std::byte* space = trampolineStringLen->RawSpace(sizeof(spacePayload));
                memcpy(space, spacePayload, sizeof(spacePayload));

                WriteOffsetValue(space + 3 + 4 + 3 + 4 + 1, reinterpret_cast<intptr_t>(stringLenAddr) + 0xA);

                const uint8_t funcPayload[] = {
                    0x48, 0x8B, 0xDB, // mov rbx, rbx
                    0x90, 0x90, // nop
                    0xE9, 0x0, 0x0, 0x0, 0x0 // jmp space2
                };

                memcpy(stringLenAddr, funcPayload, sizeof(funcPayload));

                InjectHook(reinterpret_cast<intptr_t>(stringLenAddr) + 5, space, PATCH_JUMP);
                cout << PATH_EXT_MSG;

                // Hook the AddFileEntry method to get each filepath that is loaded in the game

                // Unlike Y6, K2 has a good place to hook the call instead of hooking the first instruction of the function
                renameFilePathsFunc = get_pattern("4C 8D 44 24 20 48 8B D3 48 8B CF", -5);
                ReadCall(renameFilePathsFunc, orgY6AddFileEntry);

                InjectHook(renameFilePathsFunc, trampoline->Jump(Y6AddFileEntry));
                cout << FILE_LOAD_MSG;

                // AWB files are not passed over to the normal file entry function
                auto sprintfAWBs = get_pattern("4C 8D 4C 24 70 45 33 C0 41 8B D5 49 8B CC", -5);
                ReadCall(sprintfAWBs, orgY6SprintfAwb);

                InjectHook(sprintfAWBs, trampoline->Jump(Y6SprintfAwb));
                cout << AWB_LOAD_MSG;
                break;
            }
            case Game::VFeSports:
            {
                renameFilePathsFunc = get_pattern("40 BA 10 04 00 00 E8", 6);
                ReadCall(renameFilePathsFunc, orgVFeSAddFileEntry);

                InjectHook(renameFilePathsFunc, trampoline->Jump(VFeSAddFileEntry));
                cout << FILE_LOAD_MSG;

                // Not really sure if this affects anything, but just in case
                auto stringLenAddr = get_pattern("48 89 9C 24 A0 04 00 00 48 8B F9 B9 68 00 00 00 E8", 0xC);
                Patch(stringLenAddr, 0x68 + STR_LEN_ADD);
                cout << PATH_EXT_MSG;
                break;
            }
            case Game::YakuzaLikeADragon:
            {
                if (MH_Initialize() != MH_OK)
                {
                    cout << "Minhook initialization failed. Aborting.\n";
                    return;
                }

                renameFilePathsFunc = get_pattern("48 83 EC 28 4C 8B C2 4C 8D 4C 24 40 BA 10 04 00 00 E8", 17);
                ReadCall(renameFilePathsFunc, hookYLaDAddFileEntry);

                if (MH_CreateHook(hookYLaDAddFileEntry, &YLaDAddFileEntry, reinterpret_cast<LPVOID*>(&orgYLaDAddFileEntry)) != MH_OK)
                {
                    cout << "Hook creation failed. Aborting.\n";
                    return;
                }

                if (MH_EnableHook(hookYLaDAddFileEntry) != MH_OK)
                {
                    cout << "Hook could not be enabled. Aborting.\n";
                    return;
                }

                cout << FILE_LOAD_MSG;
                break;
            }
            case Game::Unsupported:
            default:
                cout << currentGameName << " is unsupported. Aborting." << endl;
                break;
        }
    }

    cout << "Hook function finished.\n";
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
