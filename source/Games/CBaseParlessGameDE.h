#pragma once
#include "CBaseParlessGame.h"

class CBaseParlessGameDE : public CBaseParlessGame
{
public:
	virtual std::string translate_path(std::string path, int indexOfData) override;
};
