// T5DR_Overlay.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Types.h"
#include <map>
#include "Memory.hpp"
#include "GameAdresses.h"

#include "T5DR_Overlay.h"
#include "Helpers.h"

// Uses Memory module from: https://github.com/T-vK/Memory-Hacking-Class
void T5DROverlay::QueryOverlayInfo() {
	GameAddresses gameAdresses;
	T5DRAddresses t5drAddresses;

	Memory memory;

	char processName[] = "rpcs3.exe";

	HANDLE processHandle = ProcessHandling::AttachToProcess(processName, &memory);

	gameAddr p1Address = gameAdresses.rpcs3_addr + t5drAddresses.t5dr_p1_addr;

	gameAddr p1AnimLengthAddress = p1Address + t5drAddresses.t5_currmove_anim_len_offset;

	int32_t p1AnimLength = memory.ReadInt(processHandle, p1AnimLengthAddress);
	ByteswapHelpers::SWAP_INT16(&p1AnimLength);

	std::cout << "p1 address: " << p1AnimLength << std::endl;

	std::cin.get();
}


void T5DROverlay::DisplayOverlayInfo() {
	// enigma start: display move id and name with move props
	// The move ids in the editor and also at memory address 3100909A8 are just the locations of the moves in the movelist (first move is 0, second is 1, third is 2 and so on).
	// Exceptions: Standing anim (32769) and crouching anim (32770).
	uint16_t assignedMoveId = 0;

	// Create dictionary for mapping the move ids to the moves.
	std::map<uint16_t, Move> movesMap_p1;
	//
	//for (auto& move : StructIterator<Move>(movesetBlock, offsets->move, offsets->moveCount))
	//{
	//	movesMap_p1[assignedMoveId] = move;

	//	//DEBUG_LOG("\"move_id: %u, moveId_val1: %u\";\"move_name: %s\"; \"first_active_frame: %u\";\"last_active_frame: %u\"\n", assignedMoveId, move.moveId_val1, nameBlock + move.name_addr, move.first_active_frame, move.last_active_frame);
	//	assignedMoveId++;
	//}

	//// Get current move id for p1 from memory address 3100909A8.
	//gameAddr currMoveAddress = m_game.baseAddr + playerAddress + m_game.GetValue("currmove_id");
	//uint16_t currMoveId_p1 = m_process.readInt16(currMoveAddress);
	//ByteswapHelpers::SWAP_INT16(&currMoveId_p1);

	//// Standing anim gets reporting as 32769 but has move id 0.
	//if (currMoveId_p1 == 32769) {
	//	currMoveId_p1 = 0;
	//}

	//// Crouching anim gets reporting as 32770 but has an unknown move id. @todo: Find correct move id for crouching anim.
	//if (currMoveId_p1 == 32770) {
	//	currMoveId_p1 = 0;
	//}

	//// Query whether an attack from p1 or p2 connects with the opponent (block / hit / counterhit).
	//// 0 at the address means no connection, 1 means connection.
	//gameAddr p1MoveConnectsAddress = m_game.baseAddr + m_game.ReadPtrPath("p1_attack_connects");
	//gameAddr p2MoveConnectsAddress = m_game.baseAddr + m_game.ReadPtrPath("p2_attack_connects");
	//uint32_t p1MoveConnects = m_process.readInt32(p1MoveConnectsAddress);
	//uint32_t p2MoveConnects = m_process.readInt32(p2MoveConnectsAddress);
	//ByteswapHelpers::SWAP_INT32(&p1MoveConnects);
	//ByteswapHelpers::SWAP_INT32(&p2MoveConnects);
	//int16_t frameAdvantage = 0;

	//if (p1MoveConnects != 0 || p2MoveConnects != 0) {
	//	gameAddr p1CurrMoveAnimLenAddress = m_game.baseAddr + playerAddress + m_game.GetValue("currmove_anim_len");
	//	gameAddr p2CurrMoveAnimLenAddress = m_game.baseAddr + playerAddress + m_game.GetValue("playerstruct_size") + m_game.GetValue("currmove_anim_len");
	//	uint16_t p1CurrMoveAnimLen = m_process.readInt16(p1CurrMoveAnimLenAddress);
	//	uint16_t p2CurrMoveAnimLen = m_process.readInt16(p2CurrMoveAnimLenAddress);
	//	ByteswapHelpers::SWAP_INT16(&p1CurrMoveAnimLen);
	//	ByteswapHelpers::SWAP_INT16(&p2CurrMoveAnimLen);

	//	if (p1MoveConnects != 0 && p2MoveConnects == 0) {
	//		frameAdvantage = p2CurrMoveAnimLen - (p1CurrMoveAnimLen - movesMap_p1.at(currMoveId_p1).first_active_frame);
	//	}
	//	else {
	//		/*
	//		if (p1MoveConnects == 0 && p2MoveConnects != 0) {
	//			# @todo: there is no map for player 2 right now because there is no movesetBlock for player 2.
	//			frameAdvantage = p1CurrMoveAnimLen - (p2CurrMoveAnimLen - movesMap_p2.at(currMoveId_p1).first_active_frame);
	//		}
	//		// both players' attacks connect.
	//		else {
	//			frameAdvantage = p2CurrMoveAnimLen - p1CurrMoveAnimLen;
	//		}
	//		*/
	//	}

	//}
	//
	//// @todo: Calculate frame advantage: Diff(attack_anim, recovery_anim). Plus or minus based on who is attacking.
 // 
	//DEBUG_LOG("move_id: %u, startup: %u - %u, frame adv: %i, anim_len: %u\n", currMoveId_p1, movesMap_p1.at(currMoveId_p1).first_active_frame, movesMap_p1.at(currMoveId_p1).last_active_frame, frameAdvantage, movesMap_p1.at(currMoveId_p1).anim_len);
	
	// enigma end
}

int main()
{

	T5DROverlay t5DROverlay;

	t5DROverlay.QueryOverlayInfo();
	t5DROverlay.DisplayOverlayInfo();

    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
