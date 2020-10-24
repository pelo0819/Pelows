#include "pch.h"
#include "window_manager.h"

WindowManager::WindowManager()
{
}

WindowManager::~WindowManager()
{
	delete win_title;
}

void WindowManager::SetWindowHandle(HWND h)
{
	hWnd = h;
}

HWND WindowManager::GetWindowHandle()
{
	return hWnd;
}

void WindowManager::SetWinTitle(char* c)
{
	win_title = new char[strlen(c) + 1];
	strcpy_s(win_title, strlen(c) + 1, c);
}

void WindowManager::SetProcessId(DWORD id)
{
	process_id = id;
}

DWORD WindowManager::GetProcessId()
{
	return process_id;
}

char* WindowManager::GetWinTitle()
{
	return win_title;
}

void WindowManager::PrintWinTitle()
{
	std::cout << win_title << std::endl;
}