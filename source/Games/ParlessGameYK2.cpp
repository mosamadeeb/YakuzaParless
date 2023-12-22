#include "ParlessGameYK2.h"
#include "StringHelpers.h"
#include "Utils/MemoryMgr.h"
#include "Utils/Trampoline.h"
#include "Utils/Patterns.h"
#include <MinHook.h>
#include <iostream>

using namespace hook;
using namespace Memory;

int (*ParlessGameYK2::orgYK2AddFileEntry)(int* param_1, int* param_2, char* param_3) = NULL;
uint64_t(*ParlessGameYK2::orgYK2SprintfAwb)(uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4) = NULL;

std::string ParlessGameYK2::translate_path(std::string path, int indexOfData)
{
	path = translate_path_original(path, indexOfData);

	if (firstIndexOf(path, "data/entity", indexOfData) != -1 && endsWith(path, ".txt"))
	{
		string loc = "/ja/";

		if (locale == Locale::English)
			loc = "/en/";

		path = rReplace(path, loc, "/");
	}

	return path;
}

bool ParlessGameYK2::hook_add_file()
{
	void* renameFilePathsFunc;
	uint8_t STR_LEN_ADD = 0x40;

	// Hook inside the method that calculates a string's length to add 0x20 bytes to the length
// This is needed to prevent undefined behavior when the modified path is longer than the memory allocated to it
	auto stringLenAddr = get_pattern("8B C7 3B D8 77 4E", -0x1C);

	Trampoline* trampoline = Trampoline::MakeTrampoline(GetModuleHandle(nullptr));
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
	std::cout << "Applied file path extension hook.\n";

	// Hook the AddFileEntry method to get each filepath that is loaded in the game

	// Unlike Y6, K2 has a good place to hook the call instead of hooking the first instruction of the function
	renameFilePathsFunc = get_pattern("4C 8D 44 24 20 48 8B D3 48 8B CF", -5);
	ReadCall(renameFilePathsFunc, orgYK2AddFileEntry);

	InjectHook(renameFilePathsFunc, trampoline->Jump(YK2AddFileEntry));

	// AWB files are not passed over to the normal file entry function
	auto sprintfAWBs = get_pattern("4C 8D 4C 24 70 45 33 C0 41 8B D5 49 8B CC", -5);
	ReadCall(sprintfAWBs, orgYK2SprintfAwb);

	InjectHook(sprintfAWBs, trampoline->Jump(YK2SprintfAwb));
	std::cout << "Applied AWB loading hook.\n";

	return true;
}