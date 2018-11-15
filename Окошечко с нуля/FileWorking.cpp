#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <sstream>
#include <vector>
#include <iostream>
#include <tchar.h>
#include <fstream>
#include "Options.h"
#include "OsLabGlobals.h"
#include "OsLabFunctions.h"
const TCHAR* ConfFileName = _T("config.txt");
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
	HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
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
		//std::cout << tmp << " ";
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