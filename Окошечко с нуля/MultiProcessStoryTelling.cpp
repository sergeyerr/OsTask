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
		hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, options.m*options.n * sizeof(int) * 3 + 1, SharedMemName);
		std::cout << "It is first process\n";
	}
	VievOfMem = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, options.m*options.n * sizeof(int) * 3 + 1);
	//UnmapViewOfFile(dataPtr);
	//CloseHandle(hMapping);
}

void SaveToSharedMemory() {
	VievOfMem[0] = PlacedPictures->size();
	for (int i = 0; i < PlacedPictures->size(); i++) {
		VievOfMem[1 + i * 3] = (*PlacedPictures)[i].first.first;
		VievOfMem[2 + i * 3] = (*PlacedPictures)[i].first.second;
		VievOfMem[3 + i * 3] = (*PlacedPictures)[i].second;
	}

}

void SyncWithSharedMemory() {
	if (VievOfMem == nullptr) return;
	for (int i = 0; i < VievOfMem[0]; i++) {
		if (PlacedPictures->size() <= i) {
			PlacedPictures->push_back({ { VievOfMem[1 + i * 3], VievOfMem[2 + i * 3]}, VievOfMem[3 + i * 3] });
			(*PlacedPictures)[i].first.first = VievOfMem[1 + 3 * i];
			(*PlacedPictures)[i].first.second = VievOfMem[2 + 3 * i];
			(*PlacedPictures)[i].second = VievOfMem[3 + 3 * i];
		}
		else {
			(*PlacedPictures)[i].second = VievOfMem[3 + 3 * i];
		}
	}
}