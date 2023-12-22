#include "ParlessGameY6.h"
#include "StringHelpers.h"
#include "Utils/MemoryMgr.h"
#include "Utils/Trampoline.h"
#include "Utils/Patterns.h"
#include <MinHook.h>
#include <iostream>

using namespace hook;
using namespace Memory;

int (*ParlessGameY6::orgY6AddFileEntry)(int* param_1, int* param_2, char* param_3) = NULL;
uint64_t(*ParlessGameY6::orgY6SprintfAwb)(uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4) = NULL;

std::string ParlessGameY6::translate_path(std::string path, int indexOfData)
{
	path = translate_path_original(path, indexOfData);

	if (firstIndexOf(path, "data/entity", indexOfData) != -1 && endsWith(path, ".txt"))
	{
		string loc = "/ja/";

		if (locale == Locale::English)
			loc = "/e/";

		path = rReplace(path, loc, "/");
	}

	return path;
};

bool ParlessGameY6::hook_add_file()
{
	void* renameFilePathsFunc;
	uint8_t STR_LEN_ADD = 0x40;

	// Hook inside the method that calculates a string's length to add 0x20 bytes to the length
	// This is needed to prevent undefined behavior when the modified path is longer than the memory allocated to it
	
	
	auto stringLenAddrPat = pattern("8B CD 3B D9 77 47");
	void* stringLenAddr = 0;
	bool isGOG = GetModuleHandle(L"galaxy64") != nullptr;

	//GOG
	if (!isGOG)
		stringLenAddr = get_pattern("8B CD 3B D9 77 47", -0x1C);

	Trampoline* trampoline = Trampoline::MakeTrampoline(GetModuleHandle(nullptr));
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


	//Path extension not needed in GOG
	if (!isGOG)
	{
		WriteOffsetValue(space + 3 + 2 + 4 + 3 + 1, reinterpret_cast<intptr_t>(stringLenAddr) + 8);

		const uint8_t funcPayload[] = {
			0x48, 0x8B, 0xDB, // mov rbx, rbx
			0xE9, 0x0, 0x0, 0x0, 0x0 // jmp space
		};

		memcpy(stringLenAddr, funcPayload, sizeof(funcPayload));

		InjectHook(reinterpret_cast<intptr_t>(stringLenAddr) + 3, space, PATCH_JUMP);
		std::cout << "Applied file path extension hook.\n";
	}

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

	// AWB files are not passed over to the normal file entry function
	auto sprintfAWBs = get_pattern("E8 ? ? ? ? 4C 8D 4C 24 60 45 33 C0");
	ReadCall(sprintfAWBs, orgY6SprintfAwb);

	InjectHook(sprintfAWBs, trampoline->Jump(Y6SprintfAwb));
	cout << "Applied AWB loading hook.\n";

	return true;
};