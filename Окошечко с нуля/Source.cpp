#include "OsLab.h"
#include "OsLabGlobals.h"
#include <iostream>
#include <Windows.h>
#include <Windowsx.h>
#include <tchar.h>
#include <string> 
#include <functional>
#include "TestLib.h"




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
		std::cout << "No load Image in DLL \n";
		return;
	}
	int testw, testh;
	unsigned char* arr = GetArr("1.png", &testw, &testh);
	Test1 = CreateBitmap(testw, testh, 1, 32, arr);
	arr = GetArr("2.jpg", &testw, &testh);
	Test2 = CreateBitmap(testw, testh, 1, 32, arr);
	FreeLibrary(h);
}

int main(int argc, char *argv[]) {
	std::function<void(void)> SavingFunc = CMD_Processor(argc, argv);
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