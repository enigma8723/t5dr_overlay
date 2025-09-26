#pragma once

#include "Types.h"

struct GameAddresses {
	gameAddr rpcs3_addr = 0x300000000;
};

// -- Tekken 5 DR Online (RPCS3) (NPEA00019 01.00)
struct T5DRAddresses {
	gameAddr t5dr_p1_addr = 0x10090850;
	gameAddr t5dr_p1_attack_connects_addr = 0x1008F480;
	gameAddr t5dr_p2_attack_connects_addr = 0x1008F498;
	uint16_t t5_currmove_id_offset = 0x158;
	uint16_t t5_currmove_anim_len_offset = 0x170;
	uint16_t t5_playerstruct_size_offset = 0x8D0;
	uint16_t t5_moveset_toc_address_offset = 0x50;
	uint16_t t5_moveset_toc_moves_adress_offset = 0x1E0;
	uint16_t t5_moveset_toc_cancels_adress_offset = 0x1B0;
	uint16_t t5_moveset_toc_extra_props_adress_offset = 0x1C8;
};