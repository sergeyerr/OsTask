#pragma once
#include "OsLabFunctions.h"
#include <windowsx.h>
#include <tchar.h>
#include "OsLabGlobals.h"
#include <ctime>
#include <iostream>
#include <functional>
const TCHAR* WindowClassName = _T("Mda");
bool IsGraphicWorks = true;
void RunNotepad()
{
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	ZeroMemory(&sInfo, sizeof(STARTUPINFO));
	CreateProcess(_T("C:\\Windows\\Notepad.exe"),
		NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

LRESULT CALLBACK WndProc(HWND handleWindow, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == UPDATEPLS) {
		SyncWithSharedMemory(handleWindow);
		return DefWindowProc(handleWindow, msg, wParam, lParam);
	}
	switch (msg)
	{
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
		WaitForSingleObject(ClickMutex, INFINITE);
		(*PlacedPictures)[y / options.CellSize][x / options.CellSize] = picId;
		SaveToSharedMemory(y / options.CellSize, x / options.CellSize);
		ReleaseMutex(ClickMutex);
		PostMessage(HWND_BROADCAST, UPDATEPLS, 0, 0);
		break;
	};
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_HOTKEY:
		switch (wParam) {
		case 0:
		case 1:
			PostQuitMessage(0);
			return 0;
			break;
		case 2:
			RunNotepad();
			break;
		case 3:
			WaitForSingleObject(OptionsMutex, INFINITE);
			options.TargetColor.r = rand() % 256;
			options.TargetColor.g = rand() % 256;
			options.TargetColor.b = rand() % 256;
			ReleaseMutex(OptionsMutex);
			break;
		case 4:
			if (IsGraphicWorks) {
				DWORD res = SuspendThread(GraphicThread);
				IsGraphicWorks = false;
				std::cout << "Graphics stopped\n";
			}
			else {
				ResumeThread(GraphicThread);
				IsGraphicWorks = true;
				std::cout << "Graphics resumed\n";
			}
			break;
		case 5:
			if (SetThreadPriority(GraphicThread, THREAD_PRIORITY_IDLE)) {
				std::cout << "ThreadPrior set to lowest\n";
			}
			else {
				std::cout << "Failed to set priority\n";
			}
			break;
		case 6:
			if (SetThreadPriority(GraphicThread, THREAD_PRIORITY_NORMAL)) {
				std::cout << "ThreadPrior set to normal\n";
			}
			else {
				std::cout << "Failed to set priority\n";
			}
			break;
		case 7:
			if (SetThreadPriority(GraphicThread, THREAD_PRIORITY_HIGHEST)) {
				std::cout << "ThreadPrior set to highest\n";
			}
			else {
				std::cout << "Failed to set priority\n";
			}
			break;
		}
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

bool RegisterAllStuff(HINSTANCE HandleInstance) {
	//options = Options();
	std::srand(unsigned(std::time(0)));
	OptionsMutex = CreateMutex(NULL, FALSE, NULL);
	ClickMutex = CreateMutex(NULL, FALSE, NULL);
	YellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	PlacedPictures = new std::vector<std::vector<unsigned char>>(options.n, std::vector<unsigned char>(options.m, -1));
	PicturesBitmaps = new std::vector<HBITMAP>();
	if (!RegisterCustomClass(HandleInstance)) {
		std::cout << "Can't register class";
		return false;
	};
	HandleWindow = CreateWindowEx(WS_EX_CLIENTEDGE, WindowClassName, _T("My Window"), WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, options.WindowSize.second, options.WindowSize.first, HWND_DESKTOP, NULL, HandleInstance, NULL);
	if (!HandleWindow) {
		std::cout << "Can't create WindowHandle";
		return false;
	}
	bool flag = true;
	RegisterHotKey(HandleWindow, 0, MOD_CONTROL, 0x51); //Q
	RegisterHotKey(HandleWindow, 1, 0, VK_ESCAPE);
	RegisterHotKey(HandleWindow, 2, MOD_SHIFT, 0x43); //C
	RegisterHotKey(HandleWindow, 3, 0, VK_RETURN); //enter
	RegisterHotKey(HandleWindow, 4, 0, VK_SPACE);
	RegisterHotKey(HandleWindow, 5, 0, VK_F1);
	RegisterHotKey(HandleWindow, 6, 0, VK_F2);
	RegisterHotKey(HandleWindow, 7, 0, VK_F3);
	UPDATEPLS = RegisterWindowMessage(_T("Update123"));
	ManageSharedMemory(HandleWindow);
	return flag;
}

void ClearAllStuff(HINSTANCE HandleInstance) {
	DeleteObject(YellowBrush);
	UnregisterHotKey(HandleWindow, 0);
	UnregisterHotKey(HandleWindow, 1);
	UnregisterHotKey(HandleWindow, 2);
	UnregisterHotKey(HandleWindow, 3);
	UnregisterHotKey(HandleWindow, 4);;
	UnregisterHotKey(HandleWindow, 5);
	UnregisterHotKey(HandleWindow, 6);
	UnregisterHotKey(HandleWindow, 7);
	DestroyWindow(HandleWindow);
	DeleteObject(options.LinePen);
	DeleteObject(options.BackgroundBrush);
	DeleteObject(OptionsMutex);
	DeleteObject(ClickMutex);
	UnregisterClass(WindowClassName, HandleInstance);
	GoAwayFromSharedMemory();
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
		ConfigureFromFileWinApi();
		SavingFunc = SaveWithFileWinApi;
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
			ConfigureFromFileWinApi();
			SavingFunc = SaveWithFileWinApi;
		}
	}
	return SavingFunc;
}