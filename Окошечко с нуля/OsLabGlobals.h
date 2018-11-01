#pragma once
#include <vector>
#include <Windows.h>
#include "Options.h"
#define KEK 0x1488
constexpr int CircleRadius = 15;
constexpr int BufferSize = 1024;
extern std::vector<std::vector<unsigned char>> *PlacedPictures;
extern std::vector<HBITMAP> *PicturesBitmaps;
extern HBRUSH YellowBrush;
extern Options options;


