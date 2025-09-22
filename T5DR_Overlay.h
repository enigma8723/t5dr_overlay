#pragma once




#include "Types.h"
#include "Memory.hpp"

#define DEBUG_LOG(...)  printf(__VA_ARGS__)

class T5DROverlay {

private:

	HANDLE processHandle;
	Memory memory;

	Player p1, p2;

	bool isFirstRun;

	void QueryMovelistForPlayer(Player& player, gameAddr playerAddress);
	void CreateMovelistMapForPlayer(Player& player);
	void FetchOverlayDataForPlayer(Player& attacker, Player& defender, OverlayData& overlayData);

public:

	T5DROverlay() :
		processHandle{ nullptr },
		memory{ Memory() },
		p1{ 0 },
		p2{ 0 },
		isFirstRun { true }
	{
	};

	void AttachToProcess();

	void QueryCurrentMoveInfo();
	
	void QueryMovelists();

	void CreateMovelistMap();

	void FetchOverlayData(OverlayData& p1OverlayData, OverlayData& p2OverlayData);

	void SetFirstRunFalse();


};