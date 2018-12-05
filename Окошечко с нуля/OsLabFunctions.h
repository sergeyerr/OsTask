#pragma once
#include <Windows.h>
#include "Options.h";
#include <functional>

//graphic
void PaintPicture(HDC handleDC, int x, int y, HBITMAP Pic);
void GridAndCirclesPainting(HWND handleWindow);
void BackGroundPaint(HWND handleWindow, WPARAM wParam);
//service
LRESULT CALLBACK WndProc(HWND handleWindow, UINT msg, WPARAM wParam, LPARAM lParam);
bool RegisterAllStuff(HINSTANCE HandleInstance);
void ClearAllStuff(HINSTANCE HandleInstance);
void RunNotepad();
void RunSecondPlayer();
void OptionListHandler(std::vector<std::string> &optionsList);
DWORD WINAPI BackGroundUpdater(void*);
std::function<void(void)> CMD_Processor(int argc, char *argv[]);

//file Working
void ConfigureFromStream();
void ConfigureFromFileVar();
void ConfigureFromFileMap();
void ConfigureFromFileWinApi();
void SaveWithStream();
void SaveWithFileVar();
void SaveWithFileMap();
void SaveWithFileWinApi();

//Bitmaps
void SaveBitMaps();

//MultiProcess
bool ManageSharedMemory();
bool TrySaveToSharedMemory(int i, int j);
void SyncWithSharedMemory();
void GoAwayFromSharedMemory();
bool IsItPlayerIDTurn();


