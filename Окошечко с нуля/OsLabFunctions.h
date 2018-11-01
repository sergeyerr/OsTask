#pragma once
#include <Windows.h>
#include "Options.h";
#include <functional>

//graphic
void PaintCircle(HDC handleDC, int x, int y, HBITMAP Pic);
void GridAndCirclesPainting(HWND handleWindow);

//service
LRESULT CALLBACK WndProc(HWND handleWindow, UINT msg, WPARAM wParam, LPARAM lParam);
bool RegisterAllStuff(HINSTANCE HandleInstance, HWND &WindowHandle);
void ClearAllStuff(HINSTANCE HandleInstance, HWND &WindowHandle);
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
void ManageSharedMemory();
void SaveToSharedMemory();
void SyncWithSharedMemory();


