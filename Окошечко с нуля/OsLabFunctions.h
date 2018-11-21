#pragma once
#include <Windows.h>
#include "Options.h";
#include <functional>

//graphic
void PaintPicture(HDC handleDC, int x, int y, HBITMAP Pic);
void GridAndCirclesPainting(void *);
DWORD WINAPI BackGroundPaint(void *);
//service
LRESULT CALLBACK WndProc(HWND handleWindow, UINT msg, WPARAM wParam, LPARAM lParam);
bool RegisterAllStuff(HINSTANCE HandleInstance);
void ClearAllStuff(HINSTANCE HandleInstance);
void RunNotepad();
void OptionListHandler(std::vector<std::string> &optionsList);
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


