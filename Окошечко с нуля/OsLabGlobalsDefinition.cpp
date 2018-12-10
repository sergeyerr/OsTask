#pragma once
#include <vector>
#include <Windows.h>
#include "Options.h"
#include <string>
#include "ColorStruct.h"
#ifndef _WE_WAS_HERE
std::vector<std::vector<unsigned char>> *PlacedPictures;
std::vector<HBITMAP> *PicturesBitmaps;
HBRUSH YellowBrush;
Options options;
UINT UPDATEPLS;
HANDLE OptionsMutex;
HWND HandleWindow;
HANDLE ClickMutex;
HANDLE BackGroundUpdateThread;
int PlayerID;
UINT DIEPLS;
#define _WE_WAS_HERE
#endif

