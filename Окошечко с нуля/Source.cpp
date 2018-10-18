#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <Windowsx.h>
#include <tchar.h>
#include <vector>
#include <utility>
#include <string>
#include <fstream>
#include <string>
#include <sstream> 
#include <functional>
#include <cstring>
#include "TestLib.h"
#include <ctime>
const TCHAR* WindowClassName = _T("Mda");
const TCHAR* ConfFileName = _T("config.txt");
const int CircleRadius = 15;
const int BufferSize = 1024;
std::vector<std::pair<std::pair<int, int>, HBITMAP>> *Circles;
HBRUSH YellowBrush;
HBITMAP Test1;
HBITMAP Test2;
class Options {
public:
	std::pair<int, int> WindowSize;
	int CellSize;
	HPEN LinePen;
	HBRUSH BackgroundBrush;
	COLORREF PenColor;
	COLORREF BackGroundColor;
	std::vector<std::string> icons;
	Options() {
		WindowSize = { 320, 240 };
		CellSize = 30;
		PenColor = RGB(220, 20, 60);
		LinePen = CreatePen(PS_SOLID, 1, PenColor);
		BackgroundBrush = CreateSolidBrush(RGB(0, 0, 255));
		icons = std::vector<std::string>();
		BackGroundColor = RGB(0, 0, 255);
	}
	Options(std::vector<std::string> input) {
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
	}
	std::string GetOptionsString() {
		std::stringstream ss;
		ss << WindowSize.first << ' ' << WindowSize.second << ' ' << CellSize
			<< ' ' << int(GetRValue(PenColor))<< ' ' << int(GetGValue(PenColor))<< ' ' << int(GetBValue(PenColor))
			<< ' ' << int(GetRValue(BackGroundColor)) << ' ' << int(GetGValue(BackGroundColor)) << ' ' << int(GetBValue(BackGroundColor));
		for (std::string tmp : icons) {
			ss  << ' '<< tmp;
		}
		return ss.str();
	}
};
Options options;

void RunNotepad()
{
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	ZeroMemory(&sInfo, sizeof(STARTUPINFO));
	CreateProcess(_T("C:\\Windows\\Notepad.exe"),
		NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

void PaintCircle(HDC handleDC, int x, int y, HBITMAP Pic) {
	HDC hdcMem = CreateCompatibleDC(handleDC);
	HBITMAP oldBmp = (HBITMAP)SelectObject(hdcMem, Pic);
	BitBlt(handleDC, x - options.CellSize / 2, y - options.CellSize/2, options.CellSize * 2, options.CellSize * 2, hdcMem, 0, 0, SRCCOPY);
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
	for (int i = 0; i < windowRectangle.bottom / options.CellSize + 1; i++) {
		MoveToEx(handleDC, 0, i  * options.CellSize, NULL);
		LineTo(handleDC, windowRectangle.right, i  * options.CellSize);
	}
	for (int i = 0; i < windowRectangle.right / options.CellSize + 1; i++) {
		MoveToEx(handleDC, i  * options.CellSize, 0, NULL);
		LineTo(handleDC, i  * options.CellSize, windowRectangle.bottom);
	}
	for (auto circle : *Circles) {
		PaintCircle(handleDC, circle.first.first, circle.first.second, circle.second);
	}
	EndPaint(handleWindow, &paintStruct);
	DeleteObject(handleDC);
}

LRESULT CALLBACK WndProc(HWND handleWindow, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	{
		HDC hDC = GetDC(handleWindow);
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		x = x - x % options.CellSize + options.CellSize / 2;
		y = y - y % options.CellSize + options.CellSize / 2;
		HBITMAP Pic;
		if (std::rand() % 2) Pic = Test1; else Pic = Test2;
		Circles->push_back({ { x, y }, Pic });
		InvalidateRect(handleWindow, NULL, TRUE);
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
	wndClass.hbrBackground = (HBRUSH) options.BackgroundBrush;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = WindowClassName;
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	return RegisterClassEx(&wndClass);
}

bool RegisterAllStuff(HINSTANCE HandleInstance, HWND &WindowHandle) {
	//options = Options();
	std::srand(unsigned(std::time(0)));
	YellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	Circles = new std::vector<std::pair<std::pair<int, int>, HBITMAP>>();
	if (!RegisterCustomClass(HandleInstance)) {
		std::cout << "Can't register class";
		return false;
	};
	WindowHandle = CreateWindowEx(WS_EX_CLIENTEDGE, WindowClassName, _T("My Window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, options.WindowSize.first, options.WindowSize.second,  NULL, NULL, HandleInstance, NULL);
	if (!WindowHandle) {
		std::cout << "Can't create WindowHandle";
		return false;
	}
	bool flag = true;
	flag &= RegisterHotKey(WindowHandle, 0, MOD_CONTROL, 0x51); //Q
	flag &= RegisterHotKey(WindowHandle, 1, 0, VK_ESCAPE);
	flag &= RegisterHotKey(WindowHandle, 2, MOD_SHIFT, 0x43); //C
	flag &= RegisterHotKey(WindowHandle, 3, 0, VK_RETURN); //enter
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
	if (optionsList.size() < 9) {
		std::cout << "Invalid Config\n";
		options = Options();
	}
	else {
	
		options = Options(optionsList);
	}
}

void ConfigureFromStream() {
	std::ifstream in(ConfFileName);
	std::vector<std::string> optionsList;
	std::string tmp;
	while (in.peek() != EOF) {
		in >> tmp;
		optionsList.push_back(tmp);
	}
	std::cout << "Configured from Stream!\n";
	in.close();
}

void ConfigureFromFileVar() {
	FILE* fi = _tfopen(ConfFileName, _T("r"));
	if (!fi) {
		std::cout << "Invalide File Name\n";
		options = Options();
		return;
	}
	std::vector<std::string> optionsList;
	while (!feof(fi)) {
		char tmp[BufferSize];
		fscanf(fi, "%s", &tmp);
		optionsList.push_back(tmp);
	}
	std::cout << "Configured from File Variable!\n";
	OptionListHandler(optionsList);
	fclose(fi);
}

void ConfigureFromFileMap() {
	HANDLE hFile = CreateFile(ConfFileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	DWORD dwFileSize = GetFileSize(hFile, nullptr);
	HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY,0, 0, nullptr);
	unsigned char* dataPtr = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
	if (hFile == INVALID_HANDLE_VALUE || dataPtr == nullptr) {
		std::cout << "Smth Go Wrong\n";
		options = Options();
		return;
	}
	std::stringstream ss;
	ss << dataPtr;
	std::vector<std::string> optionsList;
	std::string tmp;
	while (ss >> tmp) {
		optionsList.push_back(tmp);
	}
	std::cout << "Configured from File Mapping!\n";
	OptionListHandler(optionsList);
	UnmapViewOfFile(dataPtr);
	CloseHandle(hMapping);
	CloseHandle(hFile);
}

void ConfigureFromFileWinApi() {
	HANDLE hFile = CreateFile(ConfFileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	DWORD dwFileSize = GetFileSize(hFile, nullptr);
	std::string sData = "";
	char lpBuffer[BufferSize];
	for (int i = 0; i < BufferSize; i++) {
		lpBuffer[i] = '\0';
	}
	DWORD rSize;
	while (ReadFile(hFile, &lpBuffer, dwFileSize, &rSize, nullptr) && std::string(lpBuffer) != "") {
		sData += lpBuffer;
		for (int i = 0; i < BufferSize; i++) {
			lpBuffer[i] = '\0';
		}
	}
	std::stringstream ss;
	ss << sData;
	std::vector<std::string> optionsList;
	std::string tmp;
	while (ss >> tmp) {
		optionsList.push_back(tmp);
	}
	std::cout << "All is ok with WinApi File\n";
	OptionListHandler(optionsList);
	CloseHandle(hFile);
}

void SaveWithStream() {
	std::ofstream out("config.txt");
	std::string test = options.GetOptionsString();
	out << test;
	out.close();
	std::cout << "Saved with stream\n";
}

void SaveWithFileVar() {
	FILE* fi = _tfopen(ConfFileName, _T("w"));
	if (!fi) {
		std::cout << "Invalide File Name\n";
		return;
	}
	fprintf(fi, options.GetOptionsString().c_str());
	fclose(fi);
	std::cout << "Saved with FileVar\n";
}

void SaveWithFileMap() {
	HANDLE hFile = CreateFile(ConfFileName, GENERIC_ALL, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, BufferSize, 0);
	std::string tmp = options.GetOptionsString();
	LPTSTR dataPtr = (LPTSTR)MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, BufferSize);
	CopyMemory((PVOID)dataPtr, tmp.c_str(), sizeof(char) * tmp.size());
	bool test = FlushViewOfFile(dataPtr, 0);
	UnmapViewOfFile(dataPtr);
	CloseHandle(hMapping);
	CloseHandle(hFile);
	std::cout << "Saved with FileMap\n";
}

void SaveWithFileWinApi() {
	HANDLE hFile = CreateFile(ConfFileName, GENERIC_ALL, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	std::string tmp = options.GetOptionsString();
	char* lpBuffer = (char *)tmp.c_str();
	WriteFile(hFile, lpBuffer, sizeof(char) * tmp.size(), nullptr, nullptr);
	CloseHandle(hFile);
	std::cout << "Saved with WinApi\n";
}

void SaveBitMaps() {
	HINSTANCE h;
	unsigned char*(*GetArr) (const char*	filename, int*	width, int*	height);
	h = LoadLibrary(_T("testlib.dll"));
	if (!h)
	{
		std::cout << "No TestLib.dll\n";
		return;
	}
	GetArr = (unsigned char*(*) (const char* filename, int*	width, int*	height))
		GetProcAddress(h, "load_image");
	if (!GetArr)
	{
		std::cout << "No loadImage in DLL \n";
		return;
	}
	int testw, testh;
	unsigned char* arr = GetArr("1.png", &testw, &testh);
	Test1 = CreateBitmap(testw, testh, 1, 32, arr);
	arr = GetArr("2.jpg", &testw, &testh);
	Test2 = CreateBitmap(testw, testh, 1, 32, arr);
}

int main(int argc, char *argv[]) {
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
	SaveBitMaps();
	HINSTANCE HandleInstance = GetModuleHandle(NULL);
	HWND WindowHandle;
	MSG Msg;
	if (HandleInstance == nullptr) {
		std::cout << "Can't get Handle Instance";
		return 0;
	}
	if (!RegisterAllStuff(HandleInstance, WindowHandle)) {
		std::cout << "Registration Failed";
		return 0;
	}
	std::cout << "Registration Succeeded!\n";
	ShowWindow(WindowHandle, SW_SHOW);
	int b;
	while (b = GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	SavingFunc();
	ClearAllStuff(HandleInstance, WindowHandle);
	return b;
}