#include "KeyHook.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <stdio.h>


void Shot()
{
	HWND hDesktop = GetDesktopWindow();
	int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int top = GetSystemMetrics(SM_YVIRTUALSCREEN);

	HDC desktop_dc = GetWindowDC(hDesktop);

}