#pragma once

#include "Types.h"

#define DEBUG_LOG(...)  printf(__VA_ARGS__)

class T5DROverlay {

private:

	HANDLE processHandle;
	Memory memory;

	Byte* p1MovesetBlock;
	uint32_t moveCount;

	uint16_t p1CurrentMoveId;
	uint16_t p1AnimLength;
	uint32_t p1CurrentMoveConnects;
	Move p1CurrentMove;

	uint16_t p2CurrentMoveId;
	uint16_t p2AnimLength;
	uint32_t p2CurrentMoveConnects;
	Move p2CurrentMove;

public:

	void AttachToProcess();

	void QueryCurrentMoveInfo();
	void QueryMovelistP1();

	void DisplayOverlayInfo();


};