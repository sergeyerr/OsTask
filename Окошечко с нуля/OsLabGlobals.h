#pragma once
#include <vector>
#include <Windows.h>
#include "Options.h"
#define CircleRadius 15
#define BufferSize 1024
extern std::vector<std::pair<std::pair<int, int>, HBITMAP>> *circles;
extern HBRUSH YellowBrush;
extern HBITMAP Test1;
extern HBITMAP Test2;
extern Options options;

