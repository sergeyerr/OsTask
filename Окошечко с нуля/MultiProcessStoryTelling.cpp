#include <Windows.h>
#include <vector>
#include <iostream>
#include "OsLabGlobals.h"
#include "OsLabFunctions.h"
#define SharedMemName  (LPCWSTR)"SharedMemory"
unsigned char* VievOfMem;
HANDLE hMapping;
const int offset = 2;
void SyncWithSharedMemory(HWND handleWindow) {
	if (VievOfMem == nullptr) return;
	for (int i = 0; i < options.n; i++) {
		for (int j = 0; j < options.m; j++) {
			(*PlacedPictures)[i][j] = VievOfMem[offset + i * options.m + j];
		}
	}
}
bool ManageSharedMemory(HWND handleWindow) {
	hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,   FALSE, SharedMemName);
	if (hMapping == nullptr) {
		hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, (options.m*options.n + 1) * sizeof(unsigned char), SharedMemName);
		VievOfMem = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, (options.m*options.n+1) * sizeof(unsigned char));
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
		std::cout << (int)VievOfMem[0] << "\n";
		if (VievOfMem[0] > 1) {
			return false;
		}
		VievOfMem[0]++;
		PlayerID = 1;
		SyncWithSharedMemory(handleWindow);
	}
	return true;
	//UnmapViewOfFile(dataPtr);
	//CloseHandle(hMapping);
}

bool TrySaveToSharedMemory(int i, int j) {
	if (PlayerID != VievOfMem[1] || VievOfMem[offset + i * options.m + j] != 255) return false;
	if (VievOfMem == nullptr) return false;
	VievOfMem[1] = (VievOfMem[1] + 1) % 2;// change player;
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