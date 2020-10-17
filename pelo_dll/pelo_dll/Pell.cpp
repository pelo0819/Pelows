
#include "pch.h"
//#include "Pell.h"

extern "C" __declspec(dllexport) int plus_one(int a)
{
	std::cout << "Hello" << std::endl;
	return a + 1;
}


extern "C" __declspec(dllexport) void doGetForegroundWindow()
{
	char buf[100];
	HWND hWnd;
	hWnd = GetForegroundWindow();
	GetWindowTextA(hWnd, buf, 100);
	MessageBoxA(NULL, buf, "DoGet", MB_OK);
}


