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

	p1CurrentMoveId = (uint16_t) memory.ReadInt(processHandle, p1CurrentMoveAddress, 2);
	p1AnimLength = (uint16_t) memory.ReadInt(processHandle, p1AnimLengthAddress, 2);
	p1CurrentMoveConnects = memory.ReadInt(processHandle, p1CurrentMoveConnectsAddress, 4);

	p2CurrentMoveId = (uint16_t)memory.ReadInt(processHandle, p2CurrentMoveAddress, 2);
	p2AnimLength = (uint16_t)memory.ReadInt(processHandle, p2AnimLengthAddress, 2);
	p2CurrentMoveConnects = memory.ReadInt(processHandle, p2CurrentMoveConnectsAddress, 4);

	ByteswapHelpers::SWAP_INT16(&p1CurrentMoveId);
	ByteswapHelpers::SWAP_INT16(&p1AnimLength);
	ByteswapHelpers::SWAP_INT32(&p1CurrentMoveConnects);

	ByteswapHelpers::SWAP_INT16(&p2CurrentMoveId);
	ByteswapHelpers::SWAP_INT16(&p2AnimLength);
	ByteswapHelpers::SWAP_INT32(&p2CurrentMoveConnects);

	// moved to function: DisplayOverlayInfo()
	/*std::cout << "p1 move id: " << p1CurrentMoveId << std::endl;
	std::cout << "p1 move anim length: " << p1AnimLength << std::endl;
	std::cout << "p1 move connects?: " << p1CurrentMoveConnects << std::endl;

	std::cout << "p2 move id: " << p2CurrentMoveId << std::endl;
	std::cout << "p2 move anim length: " << p2AnimLength << std::endl;
	std::cout << "p2 move connects?: " << p2CurrentMoveConnects << std::endl;*/

	//std::cin.get();
}

void T5DROverlay::QueryMovelistP1() {
	GameAddresses gameAddresses;
	T5DRAddresses t5drAddresses;

	gameAddr p1Address = gameAddresses.rpcs3_addr + t5drAddresses.t5dr_p1_addr;

	// Address in p1's object that holds the address of p1's moveset table of contents.
	gameAddr p1MovesetTOCPointerAddress = p1Address + t5drAddresses.t5_moveset_toc_address_offset;

	// Address of moveset table of contents.
	gameAddr p1MovesetTOCAddress = memory.ReadInt(processHandle, p1MovesetTOCPointerAddress, 4);

	// Address will be in little endian. Needs to be swapped to big endian.
	ByteswapHelpers::SWAP_INT32(&p1MovesetTOCAddress);

	// Calculate address for pointer to moveset properties address. Also calculate address where moveset properties count is held.
	gameAddr p1MovesPropertiesPointerAddress = gameAddresses.rpcs3_addr + p1MovesetTOCAddress + t5drAddresses.t5_moveset_toc_moves_adress_offset;
	gameAddr p1MovesPropertiesCountAddress = gameAddresses.rpcs3_addr + p1MovesetTOCAddress + t5drAddresses.t5_moveset_toc_moves_adress_offset + 0x4;

	// Read moveset properties address at pointer. Read moveset properties count.
	// Changed return type to int because for some characters (e.g. Bryan) the value at the address is negative before the conversion to little endian.
	int p1MovesPropertiesAdress = memory.ReadInt(processHandle, p1MovesPropertiesPointerAddress, 4);
	moveCount = memory.ReadInt(processHandle, p1MovesPropertiesCountAddress, 4);

	// Address and count will be in big endian. Needs to be swapped to little endian.
	ByteswapHelpers::SWAP_INT32(&p1MovesPropertiesAdress);
	ByteswapHelpers::SWAP_INT32(&moveCount);

	SIZE_T moveSize = sizeof(Move);
	// Allocate memory for whole moveset properties.
	p1MovesetBlock = (Byte*)malloc(moveCount * moveSize);

	// Read whole moveset properties.
	memory.ReadBytes(processHandle, p1MovesetBlock, gameAddresses.rpcs3_addr + p1MovesPropertiesAdress, moveCount * moveSize);

	// Go through moveset properties and convert big endian to little endian.
	for (auto& move : StructIterator<Move>(p1MovesetBlock, moveCount))
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

// enigma start: display move id and name with move props
// The move ids in the editor and also at memory address 3100909A8 are just the locations of the moves in the movelist (first move is 0, second is 1, third is 2 and so on).
// Exceptions: Standing anim (32769) and crouching anim (32770).
void T5DROverlay::DisplayOverlayInfo() {
	
	uint16_t assignedMoveId = 0;

	// Create dictionary for mapping the move ids to the moves.
	std::map<uint16_t, Move> movesMap_p1;

	for (auto& move : StructIterator<Move>(p1MovesetBlock, moveCount))
	{
		movesMap_p1[assignedMoveId] = move;
		assignedMoveId++;
	}

	uint16_t p1CurrentMoveIdCorrected = p1CurrentMoveId;
	uint16_t p2CurrentMoveIdCorrected = p2CurrentMoveId;

	// Standing anim gets reporting as 32769 but has move id 0.
	if (p1CurrentMoveId == 32769) {
		p1CurrentMoveIdCorrected = 0;
	}

	// Crouching anim gets reporting as 32770 but has an unknown move id. @todo: Find correct move id for crouching anim.
	if (p1CurrentMoveId == 32770) {
		p1CurrentMoveIdCorrected = 0;
	}

	Move p1CurrentMove = movesMap_p1.at(p1CurrentMoveIdCorrected);

	int16_t p1FrameAdvantage = 0;
	int16_t p2FrameAdvantage = 0;

	// @todo: Add frame data overlay for p2.
	// If p1 move connects, p1 move has a hitbox and p2 is not executing the same move as before (e.g. first p1 move was blocked, now it hits).
	if (p1CurrentMoveConnects != 0 && p1CurrentMove.hitbox_location != 0 && p2CurrentMoveId != p2LastMoveId) {

		if (p1CurrentMoveConnects != 0 && p2CurrentMoveConnects == 0) {
			// @todo: first_active_frame needs to be changed to the frame the move hit on.
			p1FrameAdvantage = p2AnimLength - (p1AnimLength - p1CurrentMove.first_active_frame);
			p2FrameAdvantage = p1FrameAdvantage * -1;
		}
		else {
			/*
			if (p1CurrentMoveConnects == 0 && p2CurrentMoveConnects != 0) {
				# @todo: there is no map for player 2 right now because there is no movesetBlock for player 2.
				frameAdvantage = p1AnimLength - (p2AnimLength - movesMap_p2.at(p2CurrentMoveId).first_active_frame);
			}
			// both players' attacks connect.
			else {
				frameAdvantage = p2AnimLength - p1AnimLength;
			}
			*/
		}

		if (p1CurrentMoveId != p1LastMoveId || p2CurrentMoveId != p2LastMoveId) {

			system("cls");

			string p1PlusSymbol = "";
			if (p1FrameAdvantage > 0) {
				p1PlusSymbol = "+";
			}

			string p2PlusSymbol = "";
			if (p2FrameAdvantage > 0) {
				p2PlusSymbol = "+";
			}

			std::cout << "p1 move id: " << p1CurrentMoveId << std::endl;
			std::cout << "p1 active frames: " << p1CurrentMove.first_active_frame << " - " << p1CurrentMove.last_active_frame << std::endl;
			std::cout << "p1 frame advantage: " << p1PlusSymbol << p1FrameAdvantage << std::endl;
			std::cout << "p1 move anim length: " << p1AnimLength << std::endl;
			std::cout << "p1 move connects?: " << p1CurrentMoveConnects << std::endl;
			std::cout << "------------------------------ " << std::endl;

			std::cout << "p2 move id: " << p2CurrentMoveId << std::endl;
			//std::cout << "p2 active frames: " << p2CurrentMove.first_active_frame << " - " << p2CurrentMove.last_active_frame << std::endl;
			std::cout << "p2 frame advantage: " << p2PlusSymbol << p2FrameAdvantage << std::endl;
			std::cout << "p2 move anim length: " << p2AnimLength << std::endl;
			std::cout << "p2 move connects?: " << p2CurrentMoveConnects << std::endl;
			std::cout << "============================== " << std::endl;

			p1LastMoveId = p1CurrentMoveId;
			p2LastMoveId = p2CurrentMoveId;
		}

	}

	
  
}


void T5DROverlay::SetFirstRunFalse() {
	isFirstRun = false;
}