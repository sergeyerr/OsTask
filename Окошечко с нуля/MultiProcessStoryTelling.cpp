#include <Windows.h>
#include <vector>
#include <iostream>
#include "OsLabGlobals.h"
#include "OsLabFunctions.h"
#include <tchar.h>
#define SharedMemName  (LPCWSTR)"SharedMemory"
unsigned char* VievOfMem;
HANDLE hMapping;
const int offset = 2;
void SyncWindowText() {
	if (PlayerID == 0) {
		if (IsItPlayerIDTurn()) {
			SetWindowText(HandleWindow, _T("First player. Your Turn."));
		}
		else {
			SetWindowText(HandleWindow, _T("First player. Enemy Turn."));
		}
	}
	else {
		if (IsItPlayerIDTurn()) {
			SetWindowText(HandleWindow, _T("Second player. Your Turn."));
		}
		else {
			SetWindowText(HandleWindow, _T("Second player. Enemy Turn."));
		}
	}
}

void SyncWithSharedMemory() {
	if (VievOfMem == nullptr) return;
	for (int i = 0; i < options.n; i++) {
		for (int j = 0; j < options.m; j++) {
			(*PlacedPictures)[i][j] = VievOfMem[offset + i * options.m + j];
		}
	}
	SyncWindowText();
	InvalidateRect(HandleWindow, NULL, TRUE);
}

bool ManageSharedMemory() {
	hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,   FALSE, SharedMemName);
	if (hMapping == nullptr) {
		hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, (options.m*options.n + offset) * sizeof(unsigned char), SharedMemName);
		VievOfMem = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, (options.m*options.n + offset) * sizeof(unsigned char));
		VievOfMem[0] = 1;
		VievOfMem[1] = 0;
		PlayerID = 0;
		for (int i = 0; i < options.n; i++) {
			for (int j = 0; j < options.m; j++) {
				VievOfMem[offset + i * options.m + j] = 255;
			}
		}
		std::cout << "It is first process\n";
	}
	else {
		VievOfMem = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, (options.m*options.n + 1) * sizeof(unsigned char));
		if (VievOfMem[0] > 1) {
			return false;
		}
		VievOfMem[0]++;
		PlayerID = 1;
		SyncWithSharedMemory();

	}
	SyncWindowText();
	return true;
}

bool IsItPlayerIDTurn() {
	return PlayerID == (int) VievOfMem[1];
}

bool TrySaveToSharedMemory(int i, int j) {
	if (VievOfMem == nullptr) return false;
	VievOfMem[1] = (VievOfMem[1] + 1) % 2;// change player;
	SyncWindowText();
	unsigned char tmp = (*PlacedPictures)[i][j];
	VievOfMem[offset + i * options.m + j] = (*PlacedPictures)[i][j];
	return true;
}

void GoAwayFromSharedMemory() {
	VievOfMem[0]--;
	if (VievOfMem[0] <= 0) {
		std::cout << "It is last Process\n";
		UnmapViewOfFile(VievOfMem);
	    CloseHandle(hMapping);
	}
}