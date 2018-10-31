#pragma once
#include <vector>
#include <Windows.h>
#include "Options.h"

#ifndef _WE_WAS_HERE
std::vector<std::pair<std::pair<int, int>, HBITMAP>> *PlacedPictures;
std::vector<HBITMAP> *PicturesBitmaps;
HBRUSH YellowBrush;
Options options;
#define _WE_WAS_HERE
#endif

