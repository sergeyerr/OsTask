#pragma once
#include "OsLabGlobals.h"
#include <Windows.h>
void PaintCircle(HDC handleDC, int x, int y, HBITMAP Pic) {
	HDC hdcMem = CreateCompatibleDC(handleDC);
	HBITMAP oldBmp = (HBITMAP)SelectObject(hdcMem, Pic);
	BitBlt(handleDC, x - options.CellSize / 2, y - options.CellSize / 2, options.CellSize * 2, options.CellSize * 2, hdcMem, 0, 0, SRCCOPY);
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
	for (int i = 0; i <  options.n + 1/*windowRectangle.bottom / options.CellSize + 1*/; i++) {
		MoveToEx(handleDC, 0, i  * options.CellSize, NULL);
		LineTo(handleDC, options.CellSize * options.m /*windowRectangle.right*/, i  * options.CellSize);
	}
	for (int i = 0; i < options.m + 1/*windowRectangle.right / options.CellSize + 1*/; i++) {
		MoveToEx(handleDC, i  * options.CellSize, 0, NULL);
		LineTo(handleDC, i  * options.CellSize, options.CellSize * options.n);
	}
	for (auto circle : *PlacedPictures) {
		PaintCircle(handleDC, circle.first.first, circle.first.second, circle.second);
	}
	EndPaint(handleWindow, &paintStruct);
	DeleteObject(handleDC);
}
