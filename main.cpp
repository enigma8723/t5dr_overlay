// This file contains the 'main' function.Program execution begins and ends there.

#include <iostream>
#include "T5DR_Overlay.h"

int main()
{

	T5DROverlay t5DROverlay;

	t5DROverlay.AttachToProcess();
	t5DROverlay.QueryMovelists();
	t5DROverlay.CreateMovelistMap();

	// The logic is too slow to catch cancels that happen fast.
	// E.g. When Lili's 1,2 is inputted fast, only the frame data for 2 is displayed
	// (1 cancels into 2 after 10 frames; each loop cycle is slower than 10 frames).
	do {
		t5DROverlay.QueryCurrentMoveInfo();
		t5DROverlay.DisplayOverlayInfo();
		t5DROverlay.SetFirstRunFalse();
	} while ( true );

}