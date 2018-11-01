#pragma once
#include "OsLabFunctions.h"
#include <windowsx.h>
#include <tchar.h>
#include "OsLabGlobals.h"
#include <ctime>
#include <iostream>
#include <functional>
const TCHAR* WindowClassName = _T("Mda");
void RunNotepad()
{
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	ZeroMemory(&sInfo, sizeof(STARTUPINFO));
	CreateProcess(_T("C:\\Windows\\Notepad.exe"),
		NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

LRESULT CALLBACK WndProc(HWND handleWindow, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case KEK:
		SyncWithSharedMemory();
		break;
	case WM_LBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		x = x - x % options.CellSize + options.CellSize / 2;
		y = y - y % options.CellSize + options.CellSize / 2;
		if (x > options.m * options.CellSize || y > options.n * options.CellSize) break;
		HBITMAP Pic;
		HDC hDC = GetDC(handleWindow);
		int picId = std::rand() % PicturesBitmaps->size();
		(*PlacedPictures)[y / options.CellSize][x / options.CellSize] = picId;
		InvalidateRect(handleWindow, NULL, TRUE);
		SaveToSharedMemory(y / options.CellSize, x / options.CellSize);
		//BroadcastSystemMessage(BSF_POSTMESSAGE,(LPDWORD) BSM_APPLICATIONS, KEK, 0, 0);
		//PostMessage(HWND_BROADCAST, UPDATEPLS, 0, 0);
		break;
	};
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_HOTKEY:
		if (wParam <= 1) {
			PostQuitMessage(0);
			return 0;
		}
		else if (wParam == 2) {
			RunNotepad();
		}
		else {
			options.BackGroundColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			InvalidateRect(handleWindow, NULL, TRUE);
		}
		break;
	case WM_PAINT:
		GridAndCirclesPainting(handleWindow);
		break;
	case WM_ERASEBKGND:
		HBRUSH	brush;
		RECT windowRectangle;
		brush = CreateSolidBrush(options.BackGroundColor);
		SelectObject((HDC)wParam, brush);
		GetClientRect(handleWindow, &windowRectangle);
		options.WindowSize.first = windowRectangle.right - windowRectangle.left;
		options.WindowSize.second = windowRectangle.bottom - windowRectangle.top;
		Rectangle((HDC)wParam, windowRectangle.left, windowRectangle.top, windowRectangle.right, windowRectangle.bottom);
		DeleteObject(brush);
		break;
	case WM_CLOSE:
		DestroyWindow(handleWindow);
		break;
	default:
		return DefWindowProc(handleWindow, msg, wParam, lParam);
	}
	return 0;
};

ATOM RegisterCustomClass(HINSTANCE HandleInstance) {
	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = HandleInstance;
	wndClass.hIcon = LoadIcon(HandleInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)options.BackgroundBrush;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = WindowClassName;
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	return RegisterClassEx(&wndClass);
}

bool RegisterAllStuff(HINSTANCE HandleInstance, HWND &WindowHandle) {
	//options = Options();
	std::srand(unsigned(std::time(0)));
	YellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	PlacedPictures = new std::vector<std::vector<unsigned char>>(options.n, std::vector<unsigned char>(options.m, -1));
	PicturesBitmaps = new std::vector<HBITMAP>();
	if (!RegisterCustomClass(HandleInstance)) {
		std::cout << "Can't register class";
		return false;
	};
	WindowHandle = CreateWindowEx(WS_EX_CLIENTEDGE, WindowClassName, _T("My Window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, options.WindowSize.first, options.WindowSize.second, NULL, NULL, HandleInstance, NULL);
	if (!WindowHandle) {
		std::cout << "Can't create WindowHandle";
		return false;
	}
	bool flag = true;
	RegisterHotKey(WindowHandle, 0, MOD_CONTROL, 0x51); //Q
	RegisterHotKey(WindowHandle, 1, 0, VK_ESCAPE);
	RegisterHotKey(WindowHandle, 2, MOD_SHIFT, 0x43); //C
	RegisterHotKey(WindowHandle, 3, 0, VK_RETURN); //enter
	ManageSharedMemory();
	return flag;
}

void ClearAllStuff(HINSTANCE HandleInstance, HWND &WindowHandle) {
	DeleteObject(YellowBrush);
	UnregisterHotKey(WindowHandle, 0);
	UnregisterHotKey(WindowHandle, 1);
	UnregisterHotKey(WindowHandle, 2);
	UnregisterHotKey(WindowHandle, 3);
	DestroyWindow(WindowHandle);
	DeleteObject(options.LinePen);
	DeleteObject(options.BackgroundBrush);
	UnregisterClass(WindowClassName, HandleInstance);
}

void OptionListHandler(std::vector<std::string> &optionsList) {
	if (optionsList.size() < 11) {
		std::cout << "Invalid Config\n";
		options = Options();
	}
	else {
		options = Options(optionsList);
	}
}

std::function<void(void)> CMD_Processor(int argc, char *argv[]) {
	std::function<void(void)> SavingFunc = [] {};
	if (argc > 2) {
		std::cout << "wrong args";
		return 0;
	}
	if (argc == 1) {
		options = Options();
	}
	else {
		if (std::string(argv[1]) == "stream") {
			ConfigureFromStream();
			SavingFunc = SaveWithStream;
		}
		else if (std::string(argv[1]) == "filevar") {
			ConfigureFromFileVar();
			SavingFunc = SaveWithFileVar;
		}
		else if (std::string(argv[1]) == "mapping") {
			ConfigureFromFileMap();
			SavingFunc = SaveWithFileMap;
		}
		else if (std::string(argv[1]) == "winapi")
		{
			ConfigureFromFileWinApi();
			SavingFunc = SaveWithFileWinApi;
		}
		else {
			std::cout << "Invalide argument\n";
			options = Options();
		}
	}
	return SavingFunc;
}