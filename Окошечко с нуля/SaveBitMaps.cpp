#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <iostream>
#include "OsLabGlobals.h"
#include <string>
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
	unsigned char* arr;
	for (std::string picWay : options.icons) {
		const char * picWayC = picWay.c_str();
		arr = GetArr(picWayC, &testw, &testh);
		HBITMAP tmp = CreateBitmap(testw, testh, 1, 32, arr);
		PicturesBitmaps->push_back(tmp);
	}

	FreeLibrary(h);
}