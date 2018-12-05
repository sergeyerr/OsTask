#pragma once
#include "OsLabFunctions.h"
#include <windowsx.h>
#include <windows.h>
#include <tchar.h>
#include "OsLabGlobals.h"
#include <ctime>
#include <iostream>
#include <functional>
#include <mmsystem.h>
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

void RunSecondPlayer()
{
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	ZeroMemory(&sInfo, sizeof(STARTUPINFO));
	CreateProcess(_T(".\\perfectGame.exe"),
		NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

int CheckWinCondition(int y, int x) {
	bool res = false;
	bool tres = true;
	for (int i = 0; i < options.n; i++) {
		if ((*PlacedPictures)[i][x] != PlayerID) tres = false; // vert
	}
	res |= tres;
	tres = true;
	for (int i = 0; i < options.n; i++) {
		if ((*PlacedPictures)[y][i] != PlayerID)tres = false; // goriz
	}
	res |= tres;
	tres = true;
	for (int i = 0; i < options.n; i++) {
		if ((*PlacedPictures)[i][i] != PlayerID)tres = false; // mainD
	}
	res |= tres;
	tres = true;
	for (int i = 0; i < options.n; i++) {
		if ((*PlacedPictures)[i][options.n - i - 1] != PlayerID) tres = false; // secondD
	}
	res |= tres;
	bool isAnyFree = false;
	for (int i = 0; i < options.n; i++) {
		for (int j = 0; j < options.n; j++) {
			if ((*PlacedPictures)[i][j] == 255) isAnyFree = true; // mainD
		}
	}
	if (!res && !isAnyFree) return 2;
	return res;
}

LRESULT CALLBACK WndProc(HWND handleWindow, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == UPDATEPLS) {
		SyncWithSharedMemory();
		return DefWindowProc(handleWindow, msg, wParam, lParam);
	}
	else if (msg == DIEPLS) {
		if (IsItPlayerIDTurn()) PostQuitMessage(0);
		return 0;
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
		InvalidateRect(handleWindow, NULL, TRUE);
		WaitForSingleObject(ClickMutex, INFINITE);
		bool isItMyTurn = IsItPlayerIDTurn();
		if (isItMyTurn && (*PlacedPictures)[y / options.CellSize][x / options.CellSize] == 255) {
			(*PlacedPictures)[y / options.CellSize][x / options.CellSize] = PlayerID;
			TrySaveToSharedMemory(y / options.CellSize, x / options.CellSize);
			int gameres = CheckWinCondition(y / options.CellSize, x / options.CellSize);
			if (gameres) {
				ReleaseMutex(ClickMutex);
				//PostMessage(HWND_BROADCAST, UPDATEPLS, 0, 0);
				PostMessage(HWND_BROADCAST, DIEPLS, 0, 0);
				if (gameres == 2) MessageBox(handleWindow, _T("DRAW"), _T("GAME RESULT"), MB_OK);
				else if (PlayerID == 0) 	MessageBox(handleWindow, _T("First Player Won"), _T("GAME RESULT"), MB_OK);
				else MessageBox(handleWindow, _T("Second Player Won"), _T("GAME RESULT"), MB_OK);
				PostQuitMessage(0);
				return 0;
			}
		}
		else {
			PlaySound(_T("kasp.wav"), NULL, SND_ASYNC | SND_FILENAME);
		}
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
				DWORD res = SuspendThread(BackGroundUpdateThread);
				IsGraphicWorks = false;
				std::cout << "Graphics stopped\n";
			}
			else {
				ResumeThread(BackGroundUpdateThread);
				IsGraphicWorks = true;
				std::cout << "Graphics resumed\n";
			}
			break;
		case 5:
			if (SetThreadPriority(BackGroundUpdateThread, THREAD_PRIORITY_IDLE)) {
				std::cout << "ThreadPrior set to lowest\n";
			}
			else {
				std::cout << "Failed to set priority\n";
			}
			break;
		case 6:
			if (SetThreadPriority(BackGroundUpdateThread, THREAD_PRIORITY_NORMAL)) {
				std::cout << "ThreadPrior set to normal\n";
			}
			else {
				std::cout << "Failed to set priority\n";
			}
			break;
		case 7:
			if (SetThreadPriority(BackGroundUpdateThread, THREAD_PRIORITY_HIGHEST)) {
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
	case WM_PAINT:
		GridAndCirclesPainting(handleWindow);
		break;
	case WM_ERASEBKGND:
		BackGroundPaint(handleWindow, wParam);
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
	HandleWindow = CreateWindowEx(WS_EX_CLIENTEDGE, WindowClassName, _T("My Window"),WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, options.WindowSize.second, options.WindowSize.first, HWND_DESKTOP, NULL, HandleInstance, NULL);
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
	DIEPLS = RegisterWindowMessage(_T("Die123"));
	flag = ManageSharedMemory();
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

DWORD WINAPI BackGroundUpdater(void*) {
	while (true) {
		Sleep(30);
		int dR = 0, dG = 0, dB = 0;
		WaitForSingleObject(OptionsMutex, INFINITE);
		if (options.TargetColor == options.NowColor) {
			ReleaseMutex(OptionsMutex);
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
		ReleaseMutex(OptionsMutex);
		InvalidateRect(HandleWindow, NULL, TRUE);
	}
}