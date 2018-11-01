#include "OsLabFunctions.h"
#include <iostream>
#include <Windows.h>
#include <Windowsx.h>
#include <tchar.h>
#include <functional>

int main(int argc, char *argv[]) {
	std::function<void(void)> SavingFunc = CMD_Processor(argc, argv);
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
	SaveBitMaps();
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