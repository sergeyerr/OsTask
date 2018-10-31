#pragma once
#include "Options.h"
#include <string>

Options::Options() {
	
}

Options::Options(std::vector<std::string> input) {
	WindowSize = { std::stoi(input[0]),std::stoi(input[1]) };
	CellSize = std::stoi(input[2]);
	PenColor = RGB(std::stoi(input[3]), std::stoi(input[4]), std::stoi(input[5]));
	LinePen = CreatePen(PS_SOLID, 1, PenColor);
	BackGroundColor = RGB(std::stoi(input[6]), std::stoi(input[7]), std::stoi(input[8]));
	BackgroundBrush = CreateSolidBrush(BackGroundColor);
	icons = std::vector<std::string>();
	for (int i = 9; i < input.size(); i++) {
		icons.push_back(input[i]);
	}
};
