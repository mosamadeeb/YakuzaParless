#pragma once
#include "Maps.h"
#include "Games/CBaseParlessGame.h"
#include "Games/ParlessGameOOE.h"
#include "Games/ParlessGameY5.h"
#include "Games/ParlessGameY0.h"
#include "Games/ParlessGameYK1.h"
#include "Games/ParlessGameY6.h"
#include "Games/ParlessGameYK2.h"
#include "Games/ParlessGameJudge.h"
#include "Games/ParlessGameYLAD.h"
#include "Games/ParlessGameLJ.h"
#include "Games/ParlessGameTMWEHI.h"
#include "Games/ParlessGameIW.h"

CBaseParlessGame* get_parless_game(Game game)
{
	switch (game)
	{
	default:
	{
		return new CBaseParlessGame();
	};

	case Game::Yakuza3:
	case Game::Yakuza4:
	{
		return new ParlessGameOOE();
	};

	case Game::Yakuza0:
	{
		return new ParlessGameY0();
	}

	case Game::YakuzaKiwami:
	{
		return new ParlessGameYK1();
	}
	case Game::Yakuza5:
	{
		return new ParlessGameY5();
	};
	case Game::Yakuza6:
	{
		return new ParlessGameY6();
	};
	case Game::YakuzaKiwami2:
	{
		return new ParlessGameYK2();
	}

	case Game::Judgment:
	{
		return new ParlessGameJudge();
	};

	case Game::YakuzaLikeADragon:
	{
		return new ParlessGameYLAD();
	};
	case Game::LostJudgment:
	{
		return new ParlessGameLJ();
	};
	case Game::LikeADragonGaidenTheManWhoErasedHisName:
	{
		return new ParlessGameTMWEHI();
	};

	case Game::LikeADragonInfiniteWealthDemo:
	{
		return new ParlessGameIW();
	};
	case Game::LikeADragonInfiniteWealth:
	{
		return new ParlessGameIW();
	};
	}
}