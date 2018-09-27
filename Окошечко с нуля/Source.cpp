#include <iostream>
#include <Windows.h>
#include <Windowsx.h>
#include <tchar.h>
#include <vector>
#include <utility>
const TCHAR* WindowClassName = _T("Mda");
const int CellSize = 30;
const int CircleRadius = 15;
std::vector<std::pair<int, int>> *Circles;
COLORREF BackGroundColor = RGB(0, 0, 255);
HPEN RedPen;

void RunNotepad()
{
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	ZeroMemory(&sInfo, sizeof(STARTUPINFO));
	CreateProcess(_T("C:\\Windows\\Notepad.exe"),
		NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

void PaintCircle(HDC handleDC, int x, int y) {
	HBRUSH handleBrush = CreateSolidBrush(RGB(255, 255, 0));
	SelectObject(handleDC, handleBrush);
	Ellipse(handleDC, x - CircleRadius, y + CircleRadius, x + CircleRadius, y - CircleRadius);
	DeleteObject(handleBrush);
}

void GridAndCirclesPainting(HWND handleWindow) {
	PAINTSTRUCT paintStruct;
	RECT windowRectangle;
	HDC handleDC = BeginPaint(handleWindow, &paintStruct);
	SelectObject(handleDC, RedPen);
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
			BackGroundColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			InvalidateRect(handleWindow, NULL, TRUE);
		}
		break;
	case WM_PAINT:
		GridAndCirclesPainting(handleWindow);
		break;
	case WM_ERASEBKGND:
		HBRUSH	brush;
		RECT windowRectangle;
		brush = CreateSolidBrush(BackGroundColor);
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
	wndClass.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0, 0, 255)));
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = WindowClassName;
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	return RegisterClassEx(&wndClass);
}

bool RegisterAllStuff(HINSTANCE HandleInstance, HWND &WindowHandle) {
	RedPen = CreatePen(PS_SOLID, 1, RGB(220, 20, 60));
	if (!RegisterCustomClass(HandleInstance)) {
		std::cout << "Can't register class";
		return false;
	};
	WindowHandle = CreateWindowEx(WS_EX_CLIENTEDGE, WindowClassName, _T("My Window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, NULL, NULL, HandleInstance, NULL);
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
	DeleteObject(RedPen);
	UnregisterHotKey(WindowHandle, 0);
	UnregisterHotKey(WindowHandle, 1);
	UnregisterHotKey(WindowHandle, 2);
	UnregisterHotKey(WindowHandle, 3);
	DestroyWindow(WindowHandle);
	UnregisterClass(WindowClassName, HandleInstance);
}

int main() {
	HINSTANCE HandleInstance = GetModuleHandle(NULL);
	HWND WindowHandle;
	MSG Msg;
	Circles = new std::vector<std::pair<int, int>>();
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
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return 0;
}