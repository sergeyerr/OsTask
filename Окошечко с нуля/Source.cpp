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
const TCHAR* WindowClassName = _T("Mda");
const TCHAR* ConfFileName = _T("config.txt");
const int CellSize = 30;
const int CircleRadius = 15;

std::vector<std::pair<int, int>> *Circles;
HBRUSH YellowBrush;
class Options {
public:
	std::pair<int, int> WindowSize;
	int CellSize;
	HPEN LinePen;
	HBRUSH BackgroundBrush;
	COLORREF BackGroundColor;
	std::vector<std::string> icons;
	Options() {
		WindowSize = { 320, 240 };
		CellSize = 30;
		LinePen = CreatePen(PS_SOLID, 1, RGB(220, 20, 60));
		BackgroundBrush = CreateSolidBrush(RGB(0, 0, 255));
		icons = std::vector<std::string>();
		BackGroundColor = RGB(0, 0, 255);
	}
	Options(std::vector<std::string> input) {
		WindowSize = { std::stoi(input[0]),std::stoi(input[1]) };
		CellSize = std::stoi(input[2]);
		LinePen = CreatePen(PS_SOLID, 1, RGB(std::stoi(input[3]), std::stoi(input[4]), std::stoi(input[5])));
		BackGroundColor = RGB(std::stoi(input[6]), std::stoi(input[7]), std::stoi(input[8]));
		BackgroundBrush = CreateSolidBrush(BackGroundColor);
		icons = std::vector<std::string>();
		for (int i = 9; i < input.size(); i++) {
			icons.push_back(input[i]);
		}
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

void PaintCircle(HDC handleDC, int x, int y) {
	SelectObject(handleDC, YellowBrush);
	Ellipse(handleDC, x - CircleRadius, y + CircleRadius, x + CircleRadius, y - CircleRadius);
}

void GridAndCirclesPainting(HWND handleWindow) {
	PAINTSTRUCT paintStruct;
	RECT windowRectangle;
	HDC handleDC = BeginPaint(handleWindow, &paintStruct);
	SelectObject(handleDC, options.LinePen);
	GetClientRect(handleWindow, &windowRectangle);
	for (int i = 0; i < windowRectangle.bottom / CellSize + 1; i++) {
		MoveToEx(handleDC, 0, i  * CellSize, NULL);
		LineTo(handleDC, windowRectangle.right, i  * CellSize);
	}
	for (int i = 0; i < windowRectangle.right / CellSize + 1; i++) {
		MoveToEx(handleDC, i  * CellSize, 0, NULL);
		LineTo(handleDC, i  * CellSize, windowRectangle.bottom);
	}
	for (auto circle : *Circles) {
		PaintCircle(handleDC, circle.first, circle.second);
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
		x = x - x % CellSize + CellSize / 2;
		y = y - y % CellSize + CellSize / 2;
		Circles->push_back({ x, y });
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
	YellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	Circles = new std::vector<std::pair<int, int>>();
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

void ConfigureFromStream() {//облепи защитой 
	std::ifstream in(ConfFileName);
	std::vector<std::string> optionsList;
	std::string tmp;
	while (in.peek() != EOF) {
		in >> tmp;
		optionsList.push_back(tmp);
	}
	if (optionsList.size() < 9) {
		std::cout << "Invalid Config\n";
		options = Options();
	}
	else {
		std::cout << "Configured from Stream!\n";
		options = Options(optionsList);
	}
	in.close();
}

void ConfigureFromFileVar() {
	FILE* fi = fopen("config.txt", "rt");
	std::vector<std::string> optionsList;
	while (!feof(fi)) {
		char tmp[255];
		fscanf(fi, "%s", &tmp);
		optionsList.push_back(tmp);
	}
	if (optionsList.size() < 9) {
		std::cout << "Invalid Config\n";
		options = Options();
	}
	else {
		std::cout << "Configured from File Variable!\n";
		options = Options(optionsList);
	}
	fclose(fi);
}


int main(int argc, char *argv[]) {
	if (argc > 2) {
		std::cout << "wrong ags";
		return 0;
	}
	if (argc == 1) {
		options = Options();
	}
	else {
		if (std::string(argv[1]) == "stream") ConfigureFromStream();
		else if (std::string(argv[1]) == "fileVar") ConfigureFromFileVar();
	}
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
	ClearAllStuff(HandleInstance, WindowHandle);
	return b;
}