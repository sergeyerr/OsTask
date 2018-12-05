#include "OsLabFunctions.h"
#include <iostream>
#include <Windows.h>
#include <Windowsx.h>
#include <tchar.h>
#include <functional>
#include "OsLabGlobals.h"
#include <process.h>
int main(int argc, char *argv[]) {
	std::function<void(void)> SavingFunc = CMD_Processor(argc, argv);
	HINSTANCE HandleInstance = GetModuleHandle(NULL);
	MSG Msg;
	if (HandleInstance == nullptr) {
		std::cout << "Can't get Handle Instance";
		return 0;
	}
	if (!RegisterAllStuff(HandleInstance)) {
		std::cout << "Registration Failed";
		return 0;
	}
	SaveBitMaps();
	std::cout << "Registration Succeeded!\n";
	ShowWindow(HandleWindow, SW_SHOW);
	BackGroundUpdateThread = CreateThread(NULL, 0, &BackGroundUpdater, NULL, 0, NULL);
	RunSecondPlayer();
	int b;
	while (b = GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	SavingFunc();
	ClearAllStuff(HandleInstance);
	return b;
}