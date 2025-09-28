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
	return /*(move.hitlevel != 0) || (move.hitbox_location != 0) || */ (move.first_active_frame != 0) || (move.last_active_frame != 0);
}

bool T5DROverlay::IsMoveStanceCanceledInto(Player& player, Move move, uint16_t moveId, uint16_t& cancelDelay) {

	cancelDelay = 0;

	if (player.lastMoveId == 0 || moveId == 0) {
		return false;
	}

	Move lastMove = player.movesMap.at(player.lastMoveId);
	std::map<uint16_t, Cancel> lastMoveCancelsMap = QueryCancelsOfMove(player, lastMove.cancel_addr);

	bool moveFoundOnLastMoveCancelList{ false };

	// cancel.first is the map item's key.
	// cancel.second is the map item's value.
	for (auto const& cancel : lastMoveCancelsMap)
	{
		moveFoundOnLastMoveCancelList = (cancel.second.move_id == player.currentMoveId);

		if (moveFoundOnLastMoveCancelList) {
			cancelDelay = cancel.second.starting_frame - lastMove.first_active_frame;
			break;
		}
	}

	return (moveFoundOnLastMoveCancelList && (move.first_active_frame == 0) && (move.last_active_frame == 0));
}

uint16_t T5DROverlay::GetFramesOfFastestCancel(Player& player, uint16_t moveId) {
	std::map<uint16_t, Cancel> lastMoveCancelsMap = QueryCancelsOfMove(player, player.movesMap.at(moveId).cancel_addr);

	uint16_t frames{ 65535 };

	// cancel.first is the map item's key.
	// cancel.second is the map item's value.
	for (auto const& cancel : lastMoveCancelsMap)
	{
		if (cancel.second.starting_frame < frames) {
			frames = cancel.second.starting_frame;
		}

	}

	return frames;
}

uint16_t T5DROverlay::GetFramesOfLastCancel(Player& player, uint16_t moveId) {
	std::map<uint16_t, Cancel> lastMoveCancelsMap = QueryCancelsOfMove(player, player.movesMap.at(moveId).cancel_addr);

	// Last item in cancels list.
	auto lastItem = std::prev(lastMoveCancelsMap.end());

	return lastItem->second.starting_frame;;
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

	// debug.
	if (p1.currentMoveId == 419) {
		printf("");
	}

	// debug.
	if (p2.currentMoveId == 546) {
		printf("");
	}

	std::map<uint16_t, Cancel> p1MoveCancelMap;
	std::map<uint16_t, Cancel> p2MoveCancelMap;

	std::map<uint16_t, ExtraMoveProperty> p1MoveExtraPropsMap;
	std::map<uint16_t, ExtraMoveProperty> p2MoveExtraPropsMap;

	if (p1CurrentMove.cancel_addr != 0) {
		p1MoveCancelMap = QueryCancelsOfMove(p1, p1CurrentMove.cancel_addr);
	}

	if (p2CurrentMove.cancel_addr != 0) {
		p2MoveCancelMap = QueryCancelsOfMove(p2, p2CurrentMove.cancel_addr);
	}

	// Get extra properties list of current move.
	// Unfortunately extra property id 0x82c8 (fake recovery frames) doesn't exist in T5DR.
	// In T7 it is used to signal to the frame data display after how many frames a cancel into an attack is possible.
	// It is used to calculate frame advantage. For T5DR another way has to be found.
	if (p1CurrentMove.extra_move_property_addr != 0) {
		p1MoveExtraPropsMap = QueryExtraPropertyOfMove(p1, p1CurrentMove.extra_move_property_addr);
	}

	if (p2CurrentMove.extra_move_property_addr != 0) {
		p2MoveExtraPropsMap = QueryExtraPropertyOfMove(p2, p2CurrentMove.extra_move_property_addr);
	}

	// Given as reference to function IsMoveStanceCanceledInto().
	uint16_t p1LastMoveCancelDelay = 0;
	uint16_t p2LastMoveCancelDelay = 0;

	bool caseP1AttackConnects = (p1.currentMoveConnects != 0 && IsMoveAttack(p1CurrentMove) && p2.currentMoveConnects == 0);
	bool caseP2AttackConnects = (p2.currentMoveConnects != 0 && IsMoveAttack(p2CurrentMove) && p1.currentMoveConnects == 0);
	bool caseBothAttacksConnect = (p1.currentMoveConnects != 0 && IsMoveAttack(p1CurrentMove) && p2.currentMoveConnects != 0 && IsMoveAttack(p2CurrentMove));
	bool caseP1StanceCanceled = (p1.currentMoveConnects != 0 && IsMoveStanceCanceledInto(p1, p1CurrentMove, p1CurrentMoveIdCorrected, p1LastMoveCancelDelay) && p2.currentMoveConnects == 0);
	bool caseP2StanceCanceled = (p2.currentMoveConnects != 0 && IsMoveStanceCanceledInto(p2, p2CurrentMove, p2CurrentMoveIdCorrected, p2LastMoveCancelDelay) && p1.currentMoveConnects == 0);
	// This is not possible (because both players will be in recovery):
	//bool caseBothStanceCanceled = (p1.currentMoveConnects != 0 && IsMoveStanceCanceledInto(p1, p1CurrentMove) && p2.currentMoveConnects != 0 && IsMoveStanceCanceledInto(p2, p2CurrentMove));


	int16_t p1FrameAdvantage = 0;
	int16_t p2FrameAdvantage = 0;

	// @todo: Add frame data overlay for p2.
	// @todo: Unsure how to handle move cancels like d/f+1 -> b
	// If p1 move connects, p1 move has a hitbox and p2 is not executing the same move as before (e.g. first p1 move was blocked, now it hits).
	if ((caseP1AttackConnects || caseP1StanceCanceled) || (caseP2AttackConnects || caseP2StanceCanceled) || (caseBothAttacksConnect /* || caseBothStanceCanceled*/)) {
			
		// debug.
		if (p1.currentMoveId == 420) {
			printf("");
		}

		p1OverlayData.currentMoveId = p1.currentMoveId;
		p1OverlayData.firstActiveFrame = p1CurrentMove.first_active_frame;
		p1OverlayData.lastActiveFrame = p1CurrentMove.last_active_frame;
		p1OverlayData.animLength = GetFramesOfLastCancel(p1, p1CurrentMoveIdCorrected);
		p1OverlayData.currentMoveConnects = p1.currentMoveConnects;

		p2OverlayData.currentMoveId = p2.currentMoveId;
		p2OverlayData.firstActiveFrame = p2CurrentMove.first_active_frame;
		p2OverlayData.lastActiveFrame = p2CurrentMove.last_active_frame;
		p2OverlayData.animLength = GetFramesOfLastCancel(p2, p2CurrentMoveIdCorrected);
		p2OverlayData.currentMoveConnects = p2.currentMoveConnects;

	
		bool frameAdvantagesNeedCalculation = true;

		// @todo: first_active_frame needs to be changed to the frame the move hit on.
		// Only p1 connects with p2.
		if (frameAdvantagesNeedCalculation && caseP1AttackConnects) {
			p1.animLength = GetFramesOfLastCancel(p1, p1CurrentMoveIdCorrected);
			p2.animLength = GetFramesOfLastCancel(p1, p2CurrentMoveIdCorrected);

			p1FrameAdvantage = p2.animLength - (p1.animLength - p1CurrentMove.first_active_frame);
			p2FrameAdvantage = p1FrameAdvantage * -1;

			p2OverlayData.firstActiveFrame = 0;
			p2OverlayData.lastActiveFrame = 0;
			p2OverlayData.animLength = p2.animLength;

			p1.lastMoveId = p1CurrentMoveIdCorrected;
			p2.lastMoveId = p2CurrentMoveIdCorrected;

			frameAdvantagesNeedCalculation = false;
		}

		// Only p2 connects with p1.
		if (frameAdvantagesNeedCalculation && caseP2AttackConnects) {
			p2.animLength = GetFramesOfLastCancel(p2, p2CurrentMoveIdCorrected);
			p1.animLength = GetFramesOfLastCancel(p2, p1CurrentMoveIdCorrected);

			p2FrameAdvantage = p1.animLength - (p2.animLength - p2CurrentMove.first_active_frame);
			p1FrameAdvantage = p2FrameAdvantage * -1;

			p1OverlayData.firstActiveFrame = 0;
			p1OverlayData.lastActiveFrame = 0;
			p1OverlayData.animLength = p1.animLength;

			p1.lastMoveId = p1CurrentMoveIdCorrected;
			p2.lastMoveId = p2CurrentMoveIdCorrected;

			frameAdvantagesNeedCalculation = false;
		}

		// p1 and p2 both connect with each other.
		if (frameAdvantagesNeedCalculation && caseBothAttacksConnect) {
			p1.animLength = GetFramesOfLastCancel(p2, p1CurrentMoveIdCorrected);
			p2.animLength = GetFramesOfLastCancel(p1, p2CurrentMoveIdCorrected);

			p1FrameAdvantage = (p2.animLength - p2CurrentMove.first_active_frame) - (p1.animLength - p1CurrentMove.first_active_frame);
			p2FrameAdvantage = (p1.animLength - p1CurrentMove.first_active_frame) - (p2.animLength - p2CurrentMove.first_active_frame);

			p1OverlayData.firstActiveFrame = 0;
			p1OverlayData.lastActiveFrame = 0;
			p2OverlayData.firstActiveFrame = 0;
			p2OverlayData.lastActiveFrame = 0;
			p1OverlayData.animLength = p1.animLength;
			p2OverlayData.animLength = p2.animLength;

			p1.lastMoveId = p1CurrentMoveIdCorrected;
			p2.lastMoveId = p2CurrentMoveIdCorrected;

			frameAdvantagesNeedCalculation = false;
		}

		if (frameAdvantagesNeedCalculation && caseP1StanceCanceled) {
			p2.animLength = GetFramesOfLastCancel(p1, p2CurrentMoveIdCorrected);

			// @todo: Subtract also (last move's cancel starting frames - last move's first active frame).
			// E.g. if the current move is Paul's 420, canceled from 419, then subtract from p1's frame advantage: 16-14 = 2.
			// This is necessary because move cancels don't always happen right on or after the active frames. They can be delayed.
			// Meaning that the opponent will already have processed some recovery frames when the cancel happens.
			p1FrameAdvantage = p2.animLength - p1LastMoveCancelDelay - GetFramesOfFastestCancel(p1, p1CurrentMoveIdCorrected);
			p2FrameAdvantage = p1FrameAdvantage * -1;

			p2OverlayData.firstActiveFrame = 0;
			p2OverlayData.lastActiveFrame = 0;
			p2OverlayData.animLength = p2.animLength;

			p1.lastMoveId = 0;
			p2.lastMoveId = 0;

			frameAdvantagesNeedCalculation = false;
		}

		if (frameAdvantagesNeedCalculation && caseP2StanceCanceled) {
			p1.animLength = GetFramesOfLastCancel(p2, p1CurrentMoveIdCorrected);

			p2FrameAdvantage = p1.animLength - p2LastMoveCancelDelay - GetFramesOfFastestCancel(p2, p2CurrentMoveIdCorrected);
			p1FrameAdvantage = p2FrameAdvantage * -1;

			p1OverlayData.firstActiveFrame = 0;
			p1OverlayData.lastActiveFrame = 0;
			p1OverlayData.animLength = p1.animLength;

			p1.lastMoveId = 0;
			p2.lastMoveId = 0;

			frameAdvantagesNeedCalculation = false;
		}

		p1OverlayData.frameAdvantage = p1FrameAdvantage;
		p2OverlayData.frameAdvantage = p2FrameAdvantage;

		// @todo: Bug: The app crashes when one does a wall combo.
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

void T5DROverlay::QueryCancelsForPlayer(Player& player, gameAddr relPlayerAddress) {
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

	// Calculate address for pointer to extra properties address. Also calculate address where extra properties count is held.
	gameAddr playerCancelsPointerAddress = gameAddresses.rpcs3_addr + playerMovesetTOCAddress + t5drAddresses.t5_moveset_toc_cancels_adress_offset;
	gameAddr playerCancelsCountAddress = gameAddresses.rpcs3_addr + playerMovesetTOCAddress + t5drAddresses.t5_moveset_toc_cancels_adress_offset + 0x4;

	// Read extra properties address at pointer. Read extra properties count.
	// Changed return type to int because for some characters (e.g. Bryan) the value at the address is negative before the conversion to little endian.
	int playerCancelsAdress = memory.ReadInt(processHandle, playerCancelsPointerAddress, 4);
	player.cancelsCount = memory.ReadInt(processHandle, playerCancelsCountAddress, 4);

	// Address and count will be in big endian. Needs to be swapped to little endian.
	ByteswapHelpers::SWAP_INT32(&playerCancelsAdress);
	ByteswapHelpers::SWAP_INT32(&player.cancelsCount);

	SIZE_T cancelsSize = sizeof(Cancel);
	// Allocate memory for whole extra properties.
	player.cancelsBlock = (Byte*)malloc(player.cancelsCount * cancelsSize);

	// Read whole extra properties.
	memory.ReadBytes(processHandle, player.cancelsBlock, gameAddresses.rpcs3_addr + playerCancelsAdress, player.cancelsCount * cancelsSize);

	// Go through extra properties and convert big endian to little endian.
	for (auto& cancel : StructIterator<Cancel>(player.cancelsBlock, player.cancelsCount))
	{
		ByteswapHelpers::SWAP_INT16(&cancel.direction);
		ByteswapHelpers::SWAP_INT16(&cancel.button);
		ByteswapHelpers::SWAP_INT32(&cancel.requirements_addr);
		ByteswapHelpers::SWAP_INT32(&cancel.move_id);
		ByteswapHelpers::SWAP_INT32(&cancel.extradata_addr);
		ByteswapHelpers::SWAP_INT16(&cancel.detection_start);
		ByteswapHelpers::SWAP_INT16(&cancel.detection_end);
		ByteswapHelpers::SWAP_INT16(&cancel.starting_frame);
		ByteswapHelpers::SWAP_INT16(&cancel.cancel_option);

		// For some reason the move id is displayed with a multiplier of 65536.
		// E.g. move id 32769 is displayed as 2147549184. Move id 416 as 27525120.
		// Therefore division of the move id here by 65536 to get the actual move id.
		cancel.move_id /= 65536;
	}

	player.cancelAddress = playerCancelsAdress;
}

void T5DROverlay::QueryCancels() {
	T5DRAddresses t5drAddresses;

	QueryCancelsForPlayer(p1, t5drAddresses.t5dr_p1_addr);
	QueryCancelsForPlayer(p2, t5drAddresses.t5dr_p1_addr + t5drAddresses.t5_playerstruct_size_offset);
}


void T5DROverlay::CreateCancelsMapForPlayer(Player& player) {
	uint16_t assignedCancelId = 0;


	for (auto& cancel : StructIterator<Cancel>(player.cancelsBlock, player.cancelsCount))
	{
		player.cancelsMap[assignedCancelId] = cancel;
		assignedCancelId++;
	}
}

void T5DROverlay::CreateCancelsMap() {
	CreateCancelsMapForPlayer(p1);
	CreateCancelsMapForPlayer(p2);
}


std::map<uint16_t, Cancel> T5DROverlay::QueryCancelsOfMove(Player& player, gameAddr moveCancelAddress) {

	SIZE_T cancelSize = sizeof(Cancel);
	// Start index.
	uint32_t indexCancelOfMove = (moveCancelAddress - player.cancelAddress) / cancelSize;

	std::map<uint16_t, Cancel> moveCancelMap;
	uint32_t moveCancelIndex{ 0 };

	bool hasNextItem{ false };

	do {

		moveCancelMap[moveCancelIndex] = player.cancelsMap.at(indexCancelOfMove);

		hasNextItem = moveCancelMap[moveCancelIndex].command != 32768;  // 0x8000: End of cancel list
		moveCancelIndex++;
		indexCancelOfMove++;

	} while ((indexCancelOfMove < player.cancelsCount) && hasNextItem);

	return moveCancelMap;
}


void T5DROverlay::QueryExtraPropsForPlayer(Player& player, gameAddr relPlayerAddress) {
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

	// Calculate address for pointer to extra properties address. Also calculate address where extra properties count is held.
	gameAddr playerExtraPropertiesPointerAddress = gameAddresses.rpcs3_addr + playerMovesetTOCAddress + t5drAddresses.t5_moveset_toc_extra_props_adress_offset;
	gameAddr playerExtraPropertiesCountAddress = gameAddresses.rpcs3_addr + playerMovesetTOCAddress + t5drAddresses.t5_moveset_toc_extra_props_adress_offset + 0x4;

	// Read extra properties address at pointer. Read extra properties count.
	// Changed return type to int because for some characters (e.g. Bryan) the value at the address is negative before the conversion to little endian.
	int playerExtraPropertiesAdress = memory.ReadInt(processHandle, playerExtraPropertiesPointerAddress, 4);
	player.extraPropsCount = memory.ReadInt(processHandle, playerExtraPropertiesCountAddress, 4);

	// Address and count will be in big endian. Needs to be swapped to little endian.
	ByteswapHelpers::SWAP_INT32(&playerExtraPropertiesAdress);
	ByteswapHelpers::SWAP_INT32(&player.extraPropsCount);

	SIZE_T extraPropsSize = sizeof(ExtraMoveProperty);
	// Allocate memory for whole extra properties.
	player.extraPropsBlock = (Byte*)malloc(player.extraPropsCount * extraPropsSize);

	// Read whole extra properties.
	memory.ReadBytes(processHandle, player.extraPropsBlock, gameAddresses.rpcs3_addr + playerExtraPropertiesAdress, player.extraPropsCount * extraPropsSize);

	// Go through extra properties and convert big endian to little endian.
	for (auto& extraProp : StructIterator<ExtraMoveProperty>(player.extraPropsBlock, player.extraPropsCount))
	{
		ByteswapHelpers::SWAP_INT16(&extraProp.starting_frame);
		ByteswapHelpers::SWAP_INT16(&extraProp.id);
		ByteswapHelpers::SWAP_INT32(&extraProp.value_unsigned);
	}

	player.extraPropsAddress = playerExtraPropertiesAdress;
}

void T5DROverlay::QueryExtraProperties() {
	T5DRAddresses t5drAddresses;

	QueryExtraPropsForPlayer(p1, t5drAddresses.t5dr_p1_addr);
	QueryExtraPropsForPlayer(p2, t5drAddresses.t5dr_p1_addr + t5drAddresses.t5_playerstruct_size_offset);
}

void T5DROverlay::CreateExtraPropertiesMapForPlayer(Player& player) {
	uint16_t assignedExtraPropId = 0;


	for (auto& extraProp : StructIterator<ExtraMoveProperty>(player.extraPropsBlock, player.extraPropsCount))
	{
		player.extraPropsMap[assignedExtraPropId] = extraProp;
		assignedExtraPropId++;
	}
}


void T5DROverlay::CreateExtraPropertiesMap() {
	CreateExtraPropertiesMapForPlayer(p1);
	CreateExtraPropertiesMapForPlayer(p2);
}

// Actually there's a list from the extra property address onwards. It goes from the property at the address until
// a property with starting_frame = 0 is found.
// See function of Tekken Moveset Extractor:
// std::vector<InputMap> EditorT7::GetExtrapropListInputs(uint16_t id, VectorSet<std::string>& drawOrder)
// Editor_t7_Forms.cpp
std::map<uint16_t, ExtraMoveProperty> T5DROverlay::QueryExtraPropertyOfMove(Player& player, gameAddr moveExtraPropsAddress) {

	SIZE_T extraPropsSize = sizeof(ExtraMoveProperty);
	// Start index.
	uint32_t indexExtraPropsOfMove = (moveExtraPropsAddress - player.extraPropsAddress) / extraPropsSize;

	std::map<uint16_t, ExtraMoveProperty> moveExtraPropsMap;
	uint32_t moveExtraPropsIndex{ 0 };

	bool hasNextItem{ false };

	do {

		moveExtraPropsMap[moveExtraPropsIndex] = player.extraPropsMap.at(indexExtraPropsOfMove);

		hasNextItem = moveExtraPropsMap[moveExtraPropsIndex].starting_frame != 0;  // end of extra properties list
		moveExtraPropsIndex++;
		indexExtraPropsOfMove++;

	} while ((indexExtraPropsOfMove < player.extraPropsCount) && hasNextItem);

	return moveExtraPropsMap;
}