#pragma once




#include "Types.h"
#include "Memory.hpp"

#define DEBUG_LOG(...)  printf(__VA_ARGS__)

class T5DROverlay {

private:

	HANDLE processHandle;
	Memory memory;

	Byte* p1MovesetBlock;
	uint32_t moveCount;

	Player p1, p2;

	bool isFirstRun;

	void CreateMovelistMapForPlayer(Player& player);
	void DisplayOverlayInfoForPlayer(Player& attacker, Player& defender);

public:

	T5DROverlay() :
		processHandle{ nullptr },
		memory{ Memory() },
		p1MovesetBlock{ nullptr },
		moveCount{ 0 },
		p1{ 0 },
		p2{ 0 },
		isFirstRun { true }
	{
	};

	void AttachToProcess();

	void QueryCurrentMoveInfo();
	void QueryMovelistP1();
	void CreateMovelistMap();

	void DisplayOverlayInfo();

	void SetFirstRunFalse();


};