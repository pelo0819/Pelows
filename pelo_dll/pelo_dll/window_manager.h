#pragma once
#include "pch.h"

class WindowManager
{
private:
	HWND hWnd = NULL;
	DWORD process_id = NULL;
	char* win_title = NULL;

public:
	WindowManager();
	~WindowManager();
	void SetWindowHandle(HWND h);
	HWND GetWindowHandle();
	void SetWinTitle(char* c);
	void SetProcessId(DWORD id);
	DWORD GetProcessId();
	char* GetWinTitle();
	void PrintWinTitle();
};