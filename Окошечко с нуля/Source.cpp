#include <iostream>
#include <Windows.h>
#include <tchar.h>
const TCHAR* WindowClassName = _T("Mda");
const int CellSize = 30;
COLORREF BackGroundColor = RGB(0, 0, 255);
HPEN RedPen;
void RunNotepad()
{
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	ZeroMemory(&sInfo, sizeof(STARTUPINFO));
	puts("Starting Notepad...");
	CreateProcess(_T("C:\\Windows\\Notepad.exe"),
		NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

LRESULT CALLBACK WndProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_HOTKEY:
		if (wParam <= 1) {
			PostQuitMessage(0);
		}
		else if (wParam == 2) {
			RunNotepad();
		}
		else {
			BackGroundColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			InvalidateRect(hWindow, NULL, TRUE);
		}
		break;
	case WM_PAINT: {
		PAINTSTRUCT paintStruct;
		RECT windowRectangle;
		HDC handleDC = BeginPaint(hWindow, &paintStruct);
		SelectObject(handleDC, RedPen);
		GetClientRect(hWindow, &windowRectangle);
		for (int i = 0; i < windowRectangle.bottom / CellSize + 1; i++) {
			MoveToEx(handleDC, 0, i  * CellSize, NULL);
			LineTo(handleDC, windowRectangle.right, i  * CellSize);
		}
		for (int i = 0; i < windowRectangle.right / CellSize + 1; i++) {
			MoveToEx(handleDC, i  * CellSize, 0, NULL);
			LineTo(handleDC, i  * CellSize, windowRectangle.bottom);
		}
		EndPaint(hWindow, &paintStruct);
		DeleteObject(handleDC);
	}
		break;
	case WM_ERASEBKGND:
		HBRUSH	brush;
		RECT windowRectangle;
		brush = CreateSolidBrush(BackGroundColor);
		SelectObject((HDC)wParam, brush);
		GetClientRect(hWindow, &windowRectangle);
		Rectangle((HDC)wParam, windowRectangle.left, windowRectangle.top, windowRectangle.right, windowRectangle.bottom);
		DeleteObject(brush);
		break;
	case WM_CLOSE:
		DestroyWindow(hWindow);
		break;
	default:
		return DefWindowProc(hWindow, msg, wParam, lParam);
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
	wndClass.hbrBackground = (HBRUSH) (CreateSolidBrush(RGB(0, 0, 255)));
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = WindowClassName;
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	return RegisterClassEx(&wndClass);
}

int main() {
	HINSTANCE HandleInstance = GetModuleHandle(NULL);
	RedPen = CreatePen(PS_SOLID, 1, RGB(220, 20, 60));
	if (HandleInstance == nullptr) {
		std::cout << "Can't get Handle Instance";
		return 0;
	}
	if (!RegisterCustomClass(HandleInstance)) {
		std::cout << "Can't register class";
		return 0;
	};
	HWND WindowHandle = CreateWindowEx(WS_EX_CLIENTEDGE, WindowClassName, _T("title"), WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, NULL, NULL, HandleInstance, NULL);
	RegisterHotKey(WindowHandle, 0, MOD_CONTROL, 0x51); //Q
	RegisterHotKey(WindowHandle, 1, 0, VK_ESCAPE);
	RegisterHotKey(WindowHandle, 2, MOD_SHIFT, 0x43); //C
	RegisterHotKey(WindowHandle, 3, 0, VK_RETURN); //enter
	if (!WindowHandle) {
		std::cout << "Can't create WindowHandle";
		return 0;
	}
	ShowWindow(WindowHandle, SW_SHOW);
	//UpdateWindow(WindowHandle);
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	DeleteObject(RedPen);
	UnregisterHotKey(WindowHandle, 0);
	UnregisterHotKey(WindowHandle, 1);
	UnregisterHotKey(WindowHandle, 2);
	UnregisterHotKey(WindowHandle, 3);
	DestroyWindow(WindowHandle);
	UnregisterClass(WindowClassName, HandleInstance);
	return 0;
}