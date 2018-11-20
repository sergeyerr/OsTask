#include <Windows.h>
#include <vector>
#include <iostream>
#include "OsLabGlobals.h"
#include "OsLabFunctions.h"
#define SharedMemName  (LPCWSTR)"SharedMemory"
unsigned char* VievOfMem;
HANDLE hMapping;
void SyncWithSharedMemory(HWND handleWindow) {
	if (VievOfMem == nullptr) return;
	for (int i = 0; i < options.n; i++) {
		for (int j = 0; j < options.m; j++) {
			(*PlacedPictures)[i][j] = VievOfMem[1 + i * options.m + j];
		}
	}
}
void ManageSharedMemory(HWND handleWindow) {
	hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,   FALSE, SharedMemName);
	if (hMapping == nullptr) {
		hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, (options.m*options.n + 1) * sizeof(unsigned char), SharedMemName);
		VievOfMem = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, (options.m*options.n+1) * sizeof(unsigned char));
		VievOfMem[0] = 1;
		for (int i = 0; i < options.n; i++) {
			for (int j = 0; j < options.m; j++) {
				VievOfMem[1 + i * options.m + j] = 255;
			}
		}
		std::cout << "It is first process\n";
	}
	else {
		VievOfMem = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, (options.m*options.n + 1) * sizeof(unsigned char));
		VievOfMem[0]++;
		SyncWithSharedMemory(handleWindow);
	}

	//UnmapViewOfFile(dataPtr);
	//CloseHandle(hMapping);
}

void SaveToSharedMemory(int i, int j) {
	if (VievOfMem == nullptr) return;
	unsigned char tmp = (*PlacedPictures)[i][j];
	VievOfMem[1 + i * options.m + j] = (*PlacedPictures)[i][j];
}

void GoAwayFromSharedMemory() {
	VievOfMem[0]--;
	if (VievOfMem[0] <= 0) {
		std::cout << "It is last Process\n";
		UnmapViewOfFile(VievOfMem);
	    CloseHandle(hMapping);
	}
}