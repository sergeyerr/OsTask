#pragma once
#include "OsLabGlobals.h"
#include <Windows.h>
void PaintPicture(HDC handleDC, int x, int y, HBITMAP Pic) {
	HDC hdcMem = CreateCompatibleDC(handleDC);
	HBITMAP oldBmp = (HBITMAP)SelectObject(hdcMem, Pic);
	BitBlt(handleDC, x , y , options.CellSize , options.CellSize , hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, oldBmp);
	DeleteDC(hdcMem);
	handleDC = NULL;
	/*SelectObject(handleDC, YellowBrush);
	Ellipse(handleDC, x - CircleRadius, y + CircleRadius, x + CircleRadius, y - CircleRadius);*/
}


void GridAndCirclesPainting(void *) {
	RECT windowRectangle;
	//PAINTSTRUCT paintStruct;
	HDC handleDC = GetDC(HandleWindow);
	SelectObject(handleDC, options.LinePen);
	GetClientRect(HandleWindow, &windowRectangle);
	for (int i = 0; i <  options.n + 1/*windowRectangle.bottom / options.CellSize + 1*/; i++) {
		MoveToEx(handleDC, 0, i  * options.CellSize, NULL);
		LineTo(handleDC, options.CellSize * options.m /*windowRectangle.right*/, i  * options.CellSize);
	}
	for (int i = 0; i < options.m + 1/*windowRectangle.right / options.CellSize + 1*/; i++) {
		MoveToEx(handleDC, i  * options.CellSize, 0, NULL);
		LineTo(handleDC, i  * options.CellSize, options.CellSize * options.n);
	}
	WaitForSingleObject(ClickMutex, INFINITE);
	for (int i = 0; i < options.n; i++) {
		for (int j = 0; j < options.m; j++) {
			if ((*PlacedPictures)[i][j] == 255) continue;
			PaintPicture(handleDC, j * options.CellSize, i * options.CellSize, (*PicturesBitmaps)[(*PlacedPictures)[i][j]]);
		}
	}
	ReleaseMutex(ClickMutex);
	ReleaseDC(HandleWindow, handleDC);
	DeleteDC(handleDC);
}

void BackGroundPaint(void*) {
	HBRUSH	brush;
	RECT windowRectangle;
	GetClientRect(HandleWindow, &windowRectangle);
	GridAndCirclesPainting(NULL);
	while (true) {
		Sleep(30);
		HDC handleDC = GetDC(HandleWindow);
		int dR = 0, dG = 0, dB = 0;
		WaitForSingleObject(OptionsMutex, INFINITE);
			if (options.TargetColor == options.NowColor) {
				ReleaseMutex(OptionsMutex);
				GridAndCirclesPainting(NULL);
				continue;
			}
			if (options.TargetColor.r > options.NowColor.r) dR = 1;
			else if (options.TargetColor.r < options.NowColor.r) dR = -1;
			if (options.TargetColor.g > options.NowColor.g) dG = 1;
			else if (options.TargetColor.g < options.NowColor.g) dG = -1;
			if (options.TargetColor.b > options.NowColor.b) dB = 1;
			else if (options.TargetColor.b < options.NowColor.b) dB = -1;
			options.NowColor.r += dR;
			options.NowColor.g += dG;
			options.NowColor.b += dB;
			options.WindowSize.first = windowRectangle.right - windowRectangle.left;
			options.WindowSize.second = windowRectangle.bottom - windowRectangle.top;
		ReleaseMutex(OptionsMutex);
		brush = CreateSolidBrush(RGB(options.NowColor.r, options.NowColor.g, options.NowColor.b));
		SelectObject(handleDC, brush);
		GetClientRect(HandleWindow, &windowRectangle);
		Rectangle(handleDC, windowRectangle.left, windowRectangle.top, windowRectangle.right, windowRectangle.bottom);
		ReleaseDC(HandleWindow, handleDC);
		//RedrawWindow(HandleWindow, FALSE, FALSE, RDW_VALIDATE);
		//int a;
		DeleteDC(handleDC);
		GridAndCirclesPainting(NULL);
		DeleteObject(brush);
	}
	DeleteObject(brush);
}
