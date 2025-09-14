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

	uint16_t p1CurrentMoveId;
	uint16_t p1LastMoveId;
	uint16_t p1AnimLength;
	uint32_t p1CurrentMoveConnects;
	uint32_t p1LastMoveConnected;

	uint16_t p2CurrentMoveId;
	uint16_t p2LastMoveId;
	uint16_t p2AnimLength;
	uint32_t p2CurrentMoveConnects;
	uint32_t p2LastMoveConnected;

	bool isFirstRun;


public:

	T5DROverlay() :
		processHandle{ nullptr },
		memory{ Memory() },
		p1MovesetBlock{ nullptr },
		moveCount{ 0 },
		p1CurrentMoveId{ 0 },
		p1LastMoveId{ 0 },
		p1AnimLength{ 0 },
		p1CurrentMoveConnects{ 0 },
		p1LastMoveConnected{ 0 },
		p2CurrentMoveId{ 0 },
		p2LastMoveId{ 0 },
		p2AnimLength{ 0 },
		p2CurrentMoveConnects{ 0 },
		p2LastMoveConnected{ 0 },
		isFirstRun { true }
	{
	};

	void AttachToProcess();

	void QueryCurrentMoveInfo();
	void QueryMovelistP1();

	void DisplayOverlayInfo();

	void SetFirstRunFalse();


};