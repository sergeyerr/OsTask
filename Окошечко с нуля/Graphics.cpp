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


void GridAndCirclesPainting(HWND handleWindow) {
	PAINTSTRUCT paintStruct;
	RECT windowRectangle;
	HDC handleDC = BeginPaint(handleWindow, &paintStruct);
	SelectObject(handleDC, options.LinePen);
	GetClientRect(handleWindow, &windowRectangle);
	for (int i = 0; i < options.n + 1/*windowRectangle.bottom / options.CellSize + 1*/; i++) {
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
	EndPaint(handleWindow, &paintStruct);
	DeleteObject(handleDC);
}

void BackGroundPaint(HWND handleWindow, WPARAM wParam) {
	HBRUSH	brush;
	RECT windowRectangle;
	brush = CreateSolidBrush(RGB(options.NowColor.r, options.NowColor.g, options.NowColor.b));
	SelectObject((HDC)wParam, brush);
	GetClientRect(handleWindow, &windowRectangle);
	options.WindowSize.first = windowRectangle.right - windowRectangle.left;
	options.WindowSize.second = windowRectangle.bottom - windowRectangle.top;
	Rectangle((HDC)wParam, windowRectangle.left, windowRectangle.top, windowRectangle.right, windowRectangle.bottom);
	DeleteObject(brush);
}
