#pragma once
#ifdef TESTLIB_EXPORTS
#define TESTLIB_API __declspec(dllexport)
#else
#define TESTLIB_API __declspec(dllimport)
#endif

extern "C" TESTLIB_API char* load_image(const char*	filename, int*	width, int*	height);