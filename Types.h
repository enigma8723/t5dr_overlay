#pragma once


#include <stdint.h>

typedef uint64_t gameAddr;
typedef uint32_t gameAddr32;

typedef unsigned char Byte;

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