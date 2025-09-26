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
	bool IsMoveAttack(Move move);

	void QueryCancelsForPlayer(Player& player, gameAddr relPlayerAddress);
	void CreateCancelsMapForPlayer(Player& player);

	void QueryExtraPropsForPlayer(Player& player, gameAddr relPlayerAddress);
	void CreateExtraPropertiesMapForPlayer(Player& player);

	std::map<uint16_t, Cancel> QueryCancelsOfMove(Player& player, gameAddr moveCancelAddress);
	std::map<uint16_t, ExtraMoveProperty> QueryExtraPropertyOfMove(Player& player, gameAddr extraPropsAddress);

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

	
	void QueryCancels();

	void CreateCancelsMap();

	void QueryExtraProperties();

	void CreateExtraPropertiesMap();

};