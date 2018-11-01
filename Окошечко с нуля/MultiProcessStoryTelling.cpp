#include <Windows.h>
#include <vector>
#include <iostream>
#include "OsLabGlobals.h"
#define MaxPicSize 32*1024
#define SharedMemName  (LPCWSTR)"Global\\SharedMemory"
unsigned char* VievOfMem;
HANDLE hMapping;
void ManageSharedMemory() {
	hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,   FALSE, SharedMemName);
	if (hMapping == nullptr) {
		hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, (options.m*options.n + 1) * sizeof(unsigned char), SharedMemName);
		VievOfMem = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, (options.m*options.n+1) * sizeof(unsigned char));
		VievOfMem[0] = 1;
		for (int i = 0; i < options.n; i++) {
			for (int j = 0; j < options.m; j++) {
				VievOfMem[1 + i * options.n + j] = 255;
			}
		}
		std::cout << "It is first process\n";
	}
	else {
		VievOfMem = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, (options.m*options.n + 1) * sizeof(unsigned char));
		VievOfMem[0]++;
	}

	//UnmapViewOfFile(dataPtr);
	//CloseHandle(hMapping);
}

void SaveToSharedMemory(int i, int j) {
	if (VievOfMem == nullptr) return;
	unsigned char tmp = (*PlacedPictures)[i][j];
	VievOfMem[1 + i * options.n + j] = (*PlacedPictures)[i][j];
	for (int i = 0; i < options.n; i++) {
		for (int j = 0; j < options.m; j++) {
			std::cout << int(VievOfMem[1 + i * options.n + j]) << " ";
			//(*PlacedPictures)[i][j] = VievOfMem[1 + i * options.n + j];
		}
		std::cout << "\n";
	}
	int a = 0;
}

void SyncWithSharedMemory() {
	if (VievOfMem == nullptr) return;
	for (int i = 0; i < options.n; i++) {
		for (int j = 0; j < options.m; j++) {
			std::cout << int(VievOfMem[1 + i * options.n + j]) << " ";
			(*PlacedPictures)[i][j] = VievOfMem[1 + i * options.n + j];
		}
		std::cout << "\n";
	}
}