// T5DR_Overlay.cpp
//

#include <iostream>
#include "Types.h"
#include <map>
#include "Memory.hpp"
#include "GameAdresses.h"

#include "T5DR_Overlay.h"
#include "Helpers.h"


// Uses Memory module from: https://github.com/T-vK/Memory-Hacking-Class
void T5DROverlay::AttachToProcess() {
	char processName[] = "rpcs3.exe";

	processHandle = ProcessHandling::AttachToProcess(processName, &memory);
}



void T5DROverlay::QueryCurrentMoveInfo() {
	GameAddresses gameAddresses;
	T5DRAddresses t5drAddresses;

	gameAddr p1Address = gameAddresses.rpcs3_addr + t5drAddresses.t5dr_p1_addr;
	gameAddr p2Address = gameAddresses.rpcs3_addr + t5drAddresses.t5dr_p1_addr + t5drAddresses.t5_playerstruct_size_offset;

	gameAddr p1AnimLengthAddress = p1Address + t5drAddresses.t5_currmove_anim_len_offset;
	gameAddr p1CurrentMoveAddress = p1Address + t5drAddresses.t5_currmove_id_offset;
	gameAddr p1CurrentMoveConnectsAddress = gameAddresses.rpcs3_addr + t5drAddresses.t5dr_p1_attack_connects_addr;


	gameAddr p2AnimLengthAddress = p2Address + t5drAddresses.t5_currmove_anim_len_offset;
	gameAddr p2CurrentMoveAddress = p2Address + t5drAddresses.t5_currmove_id_offset;
	gameAddr p2CurrentMoveConnectsAddress = gameAddresses.rpcs3_addr + t5drAddresses.t5dr_p2_attack_connects_addr;

	p1.currentMoveId = (uint16_t) memory.ReadInt(processHandle, p1CurrentMoveAddress, 2);
	p1.animLength = (uint16_t) memory.ReadInt(processHandle, p1AnimLengthAddress, 2);
	p1.currentMoveConnects = memory.ReadInt(processHandle, p1CurrentMoveConnectsAddress, 4);

	p2.currentMoveId = (uint16_t)memory.ReadInt(processHandle, p2CurrentMoveAddress, 2);
	p2.animLength = (uint16_t)memory.ReadInt(processHandle, p2AnimLengthAddress, 2);
	p2.currentMoveConnects = memory.ReadInt(processHandle, p2CurrentMoveConnectsAddress, 4);

	ByteswapHelpers::SWAP_INT16(&p1.currentMoveId);
	ByteswapHelpers::SWAP_INT16(&p1.animLength);
	ByteswapHelpers::SWAP_INT32(&p1.currentMoveConnects);

	ByteswapHelpers::SWAP_INT16(&p2.currentMoveId);
	ByteswapHelpers::SWAP_INT16(&p2.animLength);
	ByteswapHelpers::SWAP_INT32(&p2.currentMoveConnects);

}

void T5DROverlay::QueryMovelistForPlayer(Player& player, gameAddr relPlayerAddress) {
	GameAddresses gameAddresses;
	T5DRAddresses t5drAddresses;

	gameAddr playerAddress = gameAddresses.rpcs3_addr + relPlayerAddress;

	// Address in p1's object that holds the address of p1's moveset table of contents.
	gameAddr playerMovesetTOCPointerAddress = playerAddress + t5drAddresses.t5_moveset_toc_address_offset;

	// Address of moveset table of contents.
	// Changed return type to int because for some characters (e.g. Bryan) the value at the address is negative before the conversion to little endian.
	int playerMovesetTOCAddress = memory.ReadInt(processHandle, playerMovesetTOCPointerAddress, 4);

	// Address will be in little endian. Needs to be swapped to big endian.
	ByteswapHelpers::SWAP_INT32(&playerMovesetTOCAddress);

	// Calculate address for pointer to moveset properties address. Also calculate address where moveset properties count is held.
	gameAddr playerMovesPropertiesPointerAddress = gameAddresses.rpcs3_addr + playerMovesetTOCAddress + t5drAddresses.t5_moveset_toc_moves_adress_offset;
	gameAddr playerMovesPropertiesCountAddress = gameAddresses.rpcs3_addr + playerMovesetTOCAddress + t5drAddresses.t5_moveset_toc_moves_adress_offset + 0x4;

	// Read moveset properties address at pointer. Read moveset properties count.
	// Changed return type to int because for some characters (e.g. Bryan) the value at the address is negative before the conversion to little endian.
	int playerMovesPropertiesAdress = memory.ReadInt(processHandle, playerMovesPropertiesPointerAddress, 4);
	player.moveCount = memory.ReadInt(processHandle, playerMovesPropertiesCountAddress, 4);

	// Address and count will be in big endian. Needs to be swapped to little endian.
	ByteswapHelpers::SWAP_INT32(&playerMovesPropertiesAdress);
	ByteswapHelpers::SWAP_INT32(&player.moveCount);

	SIZE_T moveSize = sizeof(Move);
	// Allocate memory for whole moveset properties.
	player.movesetBlock = (Byte*)malloc(player.moveCount * moveSize);

	// Read whole moveset properties.
	memory.ReadBytes(processHandle, player.movesetBlock, gameAddresses.rpcs3_addr + playerMovesPropertiesAdress, player.moveCount * moveSize);

	// Go through moveset properties and convert big endian to little endian.
	for (auto& move : StructIterator<Move>(player.movesetBlock, player.moveCount))
	{
		ByteswapHelpers::SWAP_INT32(&move.name_addr);
		ByteswapHelpers::SWAP_INT32(&move.anim_name_addr);

		ByteswapHelpers::SWAP_INT32(&move.anim_addr);
		ByteswapHelpers::SWAP_INT32(&move.vuln);
		ByteswapHelpers::SWAP_INT32(&move.hitlevel);
		ByteswapHelpers::SWAP_INT32(&move.cancel_addr);

		ByteswapHelpers::SWAP_INT16(&move.transition);

		ByteswapHelpers::SWAP_INT16(&move.moveId_val1);
		ByteswapHelpers::SWAP_INT16(&move.moveId_val2);

		ByteswapHelpers::SWAP_INT16(&move._0x1e_short);

		ByteswapHelpers::SWAP_INT32(&move.hit_condition_addr);
		ByteswapHelpers::SWAP_INT16(&move.anim_len);
		ByteswapHelpers::SWAP_INT16(&move.airborne_start);
		ByteswapHelpers::SWAP_INT16(&move.airborne_end);
		ByteswapHelpers::SWAP_INT16(&move.ground_fall);
		ByteswapHelpers::SWAP_INT32(&move.voicelip_addr);
		ByteswapHelpers::SWAP_INT32(&move.extra_move_property_addr);
		ByteswapHelpers::SWAP_INT32(&move.move_start_extraprop_addr);
		ByteswapHelpers::SWAP_INT32(&move.move_end_extraprop_addr);
		ByteswapHelpers::SWAP_INT32(&move._0x98_int);

		//ByteswapHelpers::SWAP_INT32(&move.hitbox_location); // No need to swap this field	

		ByteswapHelpers::SWAP_INT16(&move.first_active_frame);
		ByteswapHelpers::SWAP_INT16(&move.last_active_frame);
		ByteswapHelpers::SWAP_INT16(&move._0x6c_short);
		ByteswapHelpers::SWAP_INT16(&move.distance);
	}


}

void T5DROverlay::QueryMovelists() {
	T5DRAddresses t5drAddresses;

	QueryMovelistForPlayer(p1, t5drAddresses.t5dr_p1_addr);
	QueryMovelistForPlayer(p2, t5drAddresses.t5dr_p1_addr + t5drAddresses.t5_playerstruct_size_offset);
}

void T5DROverlay::CreateMovelistMapForPlayer(Player & player) {
	uint16_t assignedMoveId = 0;


	for (auto& move : StructIterator<Move>(player.movesetBlock, player.moveCount))
	{
		player.movesMap[assignedMoveId] = move;
		assignedMoveId++;
	}
}


void T5DROverlay::CreateMovelistMap() {
	CreateMovelistMapForPlayer(p1);
	CreateMovelistMapForPlayer(p2);
}

bool T5DROverlay::IsMoveAttack(Move move) {
	return (move.hitlevel != 0) || (move.hitbox_location != 0) || (move.first_active_frame != 0) || (move.last_active_frame != 0);
}


void T5DROverlay::FetchOverlayData(OverlayData& p1OverlayData, OverlayData& p2OverlayData) {

		uint16_t p1CurrentMoveIdCorrected = p1.currentMoveId;
		uint16_t p2CurrentMoveIdCorrected = p2.currentMoveId;

		// Standing anim gets reporting as 32769 but has move id 0.
		if (p1.currentMoveId == 32769) {
			p1CurrentMoveIdCorrected = 0;
		}

		// Crouching anim gets reporting as 32770 but has an unknown move id. @todo: Find correct move id for crouching anim.
		if (p1.currentMoveId == 32770) {
			p1CurrentMoveIdCorrected = 0;
		}

		// Catch all unknown move aliases.
		if (p1.currentMoveId >= 32768) {
			p1CurrentMoveIdCorrected = 0;
		}

		// Standing anim gets reporting as 32769 but has move id 0.
		if (p2.currentMoveId == 32769) {
			p2CurrentMoveIdCorrected = 0;
		}

		// Crouching anim gets reporting as 32770 but has an unknown move id. @todo: Find correct move id for crouching anim.
		if (p2.currentMoveId == 32770) {
			p2CurrentMoveIdCorrected = 0;
		}

		// Catch all unknown move aliases.
		if (p2.currentMoveId >= 32768) {
			p2CurrentMoveIdCorrected = 0;
		}

		Move p1CurrentMove = p1.movesMap.at(p1CurrentMoveIdCorrected);
		Move p2CurrentMove = p2.movesMap.at(p2CurrentMoveIdCorrected);

		int16_t p1FrameAdvantage = 0;
		int16_t p2FrameAdvantage = 0;

		// @todo: Add frame data overlay for p2.
		// @todo: Unsure how to handle move cancels like d/f+1 -> b
		// If p1 move connects, p1 move has a hitbox and p2 is not executing the same move as before (e.g. first p1 move was blocked, now it hits).
		if ((p1.currentMoveConnects != 0 && IsMoveAttack(p1CurrentMove))
			|| (p2.currentMoveConnects != 0 && IsMoveAttack(p2CurrentMove))) {
			
			p1OverlayData.currentMoveId = p1.currentMoveId;
			p1OverlayData.firstActiveFrame = p1CurrentMove.first_active_frame;
			p1OverlayData.lastActiveFrame = p1CurrentMove.last_active_frame;
			p1OverlayData.animLength = p1.animLength;
			p1OverlayData.currentMoveConnects = p1.currentMoveConnects;

			p2OverlayData.currentMoveId = p2.currentMoveId;
			p2OverlayData.firstActiveFrame = p2CurrentMove.first_active_frame;
			p2OverlayData.lastActiveFrame = p2CurrentMove.last_active_frame;
			p2OverlayData.animLength = p2.animLength;
			p2OverlayData.currentMoveConnects = p2.currentMoveConnects;

			// @todo: first_active_frame needs to be changed to the frame the move hit on.
			// Only p1 connects with p2.
			if ((p1.currentMoveConnects != 0 && IsMoveAttack(p1CurrentMove))
				&& (p2.currentMoveConnects == 0)) {
				p1FrameAdvantage = p2.animLength - (p1.animLength - p1CurrentMove.first_active_frame);
				p2FrameAdvantage = p1FrameAdvantage * -1;
			}
			else {
				// Only p2 connects with p1.
				if ((p1.currentMoveConnects == 0)
					&& (p2.currentMoveConnects != 0 && IsMoveAttack(p2CurrentMove))) {
					p2FrameAdvantage = p1.animLength - (p2.animLength - p2CurrentMove.first_active_frame);
					p1FrameAdvantage = p2FrameAdvantage * -1;
				}
				// p1 and p2 both connect with each other.
				else {
					p1FrameAdvantage = (p2.animLength - p2CurrentMove.first_active_frame) - (p1.animLength - p1CurrentMove.first_active_frame);
					p2FrameAdvantage = (p1.animLength - p1CurrentMove.first_active_frame) - (p2.animLength - p2CurrentMove.first_active_frame);
				}
			}


			if (p1FrameAdvantage == 10) {
				printf("");
			}

			p1OverlayData.frameAdvantage = p1FrameAdvantage;
			p2OverlayData.frameAdvantage = p2FrameAdvantage;

			p1.lastMoveId = p1.currentMoveId;
			p2.lastMoveId = p2.currentMoveId;
		}

}


// enigma start: display move id and name with move props
// The move ids in the editor and also at memory address 3100909A8 are just the locations of the moves in the movelist (first move is 0, second is 1, third is 2 and so on).
// Exceptions: Standing anim (32769) and crouching anim (32770).
/*void T5DROverlay::FetchOverlayData(OverlayData& p1OverlayData, OverlayData& p2OverlayData) {
	
	FetchOverlayDataForPlayer(p1, p2, p1OverlayData);
	FetchOverlayDataForPlayer(p2, p1, p2OverlayData);
}*/


void T5DROverlay::SetFirstRunFalse() {
	isFirstRun = false;
}