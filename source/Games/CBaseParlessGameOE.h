#pragma once
#include "CBaseParlessGame.h"

class CBaseParlessGameOE : public CBaseParlessGame
{
public:
	std::string translate_path_original(std::string path, int indexOfData);
};