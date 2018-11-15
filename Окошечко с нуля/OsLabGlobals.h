#pragma once
#include <vector>
#include <Windows.h>
#include "Options.h"
#include "ColorStruct.h"
constexpr int CircleRadius = 15;
constexpr int BufferSize = 1024;
extern UINT UPDATEPLS;
extern std::vector<std::vector<unsigned char>> *PlacedPictures;
extern std::vector<HBITMAP> *PicturesBitmaps;
extern HBRUSH YellowBrush;
extern Options options;
extern HANDLE BackColorMutex;
