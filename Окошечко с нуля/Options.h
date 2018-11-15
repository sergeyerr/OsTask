#pragma once
#include <utility>
#include <Windows.h>
#include <vector>
#include <sstream>
#include "ColorStruct.h"
class Options {
public:
	std::pair<int, int> WindowSize;
	int CellSize;
	HPEN LinePen;
	HBRUSH BackgroundBrush;
	COLORREF PenColor;
	//COLORREF BackGroundColor;
	Color TargetColor;
	Color NowColor;
	int n, m;
	std::vector<std::string> icons;
	Options() {
		WindowSize = { 320, 240 };
		CellSize = 30;
		TargetColor = NowColor = { 0, 0, 255 };
		PenColor = RGB(220, 20, 60);
		LinePen = CreatePen(PS_SOLID, 1, PenColor);
		BackgroundBrush = CreateSolidBrush(RGB(0, 0, 255));
		icons = std::vector<std::string>();
		TargetColor = NowColor = { 0, 0, 255 };
		n = 100;
		m = 200;
	};
	Options(std::vector<std::string> input) {
		WindowSize = { std::stoi(input[0]),std::stoi(input[1]) };
		CellSize = std::stoi(input[2]);
		PenColor = RGB(std::stoi(input[3]), std::stoi(input[4]), std::stoi(input[5]));
		LinePen = CreatePen(PS_SOLID, 1, PenColor);
		TargetColor = NowColor = { std::stoi(input[6]), std::stoi(input[7]), std::stoi(input[8]) };
		BackgroundBrush = CreateSolidBrush(RGB(NowColor.r, NowColor.g, NowColor.b));
		icons = std::vector<std::string>();
		for (int i = 9; i < input.size() - 2; i++) {
			icons.push_back(input[i]);
		}
		n = std::stoi(input[input.size() - 2]);
		m = std::stoi(input[input.size() - 1]);
	};
	std::string GetOptionsString() {
		std::stringstream ss;
		ss << WindowSize.first << " " << WindowSize.second << " " << CellSize
			<< " " << int(GetRValue(PenColor)) << " " << int(GetGValue(PenColor)) << " " << int(GetBValue(PenColor))
			<< " " << NowColor.r << " " << NowColor.g << " " << NowColor.b;
		for (std::string tmp : icons) {
			ss << " " << tmp;
		}
		ss << " " << n << " " << m;
		return ss.str();
	}
};
