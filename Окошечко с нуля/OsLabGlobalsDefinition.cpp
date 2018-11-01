#pragma once
#include <vector>
#include <Windows.h>
#include "Options.h"
#include <string>
#ifndef _WE_WAS_HERE
std::vector<std::pair<std::pair<int, int>, int>> *PlacedPictures;
std::vector<HBITMAP> *PicturesBitmaps;
HBRUSH YellowBrush;
Options options;
#define _WE_WAS_HERE
#endif

