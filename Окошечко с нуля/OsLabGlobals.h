#pragma once
#include <vector>
#include <Windows.h>
#include "Options.h"
constexpr int CircleRadius = 15;
constexpr int BufferSize = 1024;
extern std::vector<std::pair<std::pair<int, int>, HBITMAP>> *PlacedPictures;
extern std::vector<HBITMAP> *PicturesBitmaps;
extern HBRUSH YellowBrush;
extern Options options;

