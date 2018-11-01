#pragma once
#include <vector>
#include <Windows.h>
#include "Options.h"
#include <string>
#ifndef _WE_WAS_HERE
std::vector<std::vector<unsigned char>> *PlacedPictures;
std::vector<HBITMAP> *PicturesBitmaps;
HBRUSH YellowBrush;
Options options;
#define _WE_WAS_HERE
#endif

