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


void T5DROverlay::DisplayOverlayInfoForPlayer(Player& attacker, Player& defender) {

	if (attacker.currentMoveConnects != 0) {

		uint16_t attackerCurrentMoveIdCorrected = attacker.currentMoveId;
		uint16_t defenderCurrentMoveIdCorrected = defender.currentMoveId;

		// Standing anim gets reporting as 32769 but has move id 0.
		if (attacker.currentMoveId == 32769) {
			attackerCurrentMoveIdCorrected = 0;
		}

		// Crouching anim gets reporting as 32770 but has an unknown move id. @todo: Find correct move id for crouching anim.
		if (attacker.currentMoveId == 32770) {
			attackerCurrentMoveIdCorrected = 0;
		}

		Move attackerCurrentMove = attacker.movesMap.at(attackerCurrentMoveIdCorrected);

		int16_t attackerFrameAdvantage = 0;
		int16_t defenderFrameAdvantage = 0;

		// @todo: Add frame data overlay for p2.
		// If p1 move connects, p1 move has a hitbox and p2 is not executing the same move as before (e.g. first p1 move was blocked, now it hits).
		if (attackerCurrentMove.hitbox_location != 0) {
			// @todo: first_active_frame needs to be changed to the frame the move hit on.
			attackerFrameAdvantage = defender.animLength - (attacker.animLength - attackerCurrentMove.first_active_frame);
			defenderFrameAdvantage = attackerFrameAdvantage * -1;

			system("cls");

			string attackerPlusSymbol = "";
			if (attackerFrameAdvantage > 0) {
				attackerPlusSymbol = "+";
			}

			string defenderPlusSymbol = "";
			if (defenderFrameAdvantage > 0) {
				defenderPlusSymbol = "+";
			}

			std::cout << "p1 move id: " << attacker.currentMoveId << std::endl;
			std::cout << "p1 active frames: " << attackerCurrentMove.first_active_frame << " - " << attackerCurrentMove.last_active_frame << std::endl;
			std::cout << "p1 frame advantage: " << attackerPlusSymbol << attackerFrameAdvantage << std::endl;
			std::cout << "p1 move anim length: " << attacker.animLength << std::endl;
			std::cout << "p1 move connects?: " << attacker.currentMoveConnects << std::endl;
			std::cout << "------------------------------ " << std::endl;

			std::cout << "p2 move id: " << defender.currentMoveId << std::endl;
			std::cout << "p2 frame advantage: " << defenderPlusSymbol << defenderFrameAdvantage << std::endl;
			std::cout << "p2 move anim length: " << defender.animLength << std::endl;
			std::cout << "p2 move connects?: " << defender.currentMoveConnects << std::endl;
			std::cout << "============================== " << std::endl;

			attacker.lastMoveId = attacker.currentMoveId;
			defender.lastMoveId = defender.currentMoveId;
		}
	}

}


// enigma start: display move id and name with move props
// The move ids in the editor and also at memory address 3100909A8 are just the locations of the moves in the movelist (first move is 0, second is 1, third is 2 and so on).
// Exceptions: Standing anim (32769) and crouching anim (32770).
void T5DROverlay::DisplayOverlayInfo() {
	
	DisplayOverlayInfoForPlayer(p1, p2);	
	DisplayOverlayInfoForPlayer(p2, p1);
}


void T5DROverlay::SetFirstRunFalse() {
	isFirstRun = false;
}