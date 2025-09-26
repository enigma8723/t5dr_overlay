#pragma once


#include <stdint.h>
#include <map>

typedef uint64_t gameAddr;
typedef uint32_t gameAddr32;

typedef unsigned char Byte;

// Taken from: https://github.com/Kiloutre/TKMovesets
struct Move
{
	gameAddr32 name_addr;
	gameAddr32 anim_name_addr;
	gameAddr32 anim_addr;
	uint32_t vuln;
	uint32_t hitlevel;
	gameAddr32 cancel_addr;
	uint16_t transition;
	uint16_t moveId_val2; // Clearly related to current character ID
	uint16_t moveId_val1; // Clearly related to move ID
	int16_t _0x1e_short;
	gameAddr32 hit_condition_addr;
	int16_t anim_len;
	uint16_t airborne_start;
	uint16_t airborne_end;
	uint16_t ground_fall;
	gameAddr32 voicelip_addr; // Can be NULL
	gameAddr32 extra_move_property_addr; // Can be NULL
	gameAddr32 move_start_extraprop_addr; // Can be NULL
	gameAddr32 move_end_extraprop_addr; // Can be NULL
	int32_t _0x98_int; // facing/extras
	uint32_t hitbox_location;
	uint16_t first_active_frame;
	uint16_t last_active_frame;
	int16_t _0x6c_short;
	uint16_t distance;
};

struct ExtraMoveProperty
{
	uint16_t starting_frame;
	uint16_t id;
	union
	{
		uint32_t value_unsigned;
		int32_t value_signed;
		float value_float;
	};
};

struct Player {
	uint16_t currentMoveId;
	uint16_t lastMoveId;
	uint16_t animLength;
	uint32_t currentMoveConnects;
	uint32_t lastMoveConnected;
	gameAddr extraPropsAddress; // beginning of extra properties list

	Byte* movesetBlock;
	uint32_t moveCount;

	Byte* extraPropsBlock;
	uint32_t extraPropsCount;

	// Create dictionary for mapping the move ids to the moves.
	std::map<uint16_t, Move> movesMap;
	std::map<uint16_t, ExtraMoveProperty> extraPropsMap;
};

struct OverlayData {
	uint16_t currentMoveId;
	uint16_t firstActiveFrame;
	uint16_t lastActiveFrame;
	int16_t frameAdvantage;
	uint32_t animLength;
	uint32_t currentMoveConnects;
};
