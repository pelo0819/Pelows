#include "pch.h"
#include "pelo_keyboard_helpers.h"

class KeyLogInfo 
{
private:
	HWND hWnd = NULL;
	HHOOK hHook = NULL;
	DWORD process_id = NULL;
	char* win_title = NULL;

public:
	KeyLogInfo() 
	{
	}

	~KeyLogInfo() 
	{
		delete win_title;
	}

	void SetWindowHandle(HWND h)
	{
		hWnd = h;
	}

	HWND GetWindowHandle()
	{
		return hWnd;
	}

	void SetWinTitle(char* c) 
	{
		win_title = new char[strlen(c) + 1];
		strcpy_s(win_title, strlen(c) + 1, c);
	}

	void SetProcessId(DWORD id)
	{
		process_id = id;
	}

	DWORD GetProcessId()
	{
		return process_id;
	}

	char* GetWinTitle()
	{
		return win_title;
	}

	void PrintWinTitle()
	{
		std::cout << win_title << std::endl;
	}

	void StartHook() 
	{
		/*if (hWnd == NULL) 
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
		}*/
	}


	bool EndHook()
	{
		return UnhookWindowsHookEx(hHook);
	}

};


KeyLogInfo* key_log_info;
extern "C" __declspec(dllexport) void init_key_log_info()
{
	key_log_info = new KeyLogInfo();
}

extern "C" __declspec(dllexport) void exit_key_log_info()
{
	delete key_log_info;
}

extern "C" __declspec(dllexport) void print_win_title()
{
	key_log_info->PrintWinTitle();
}

extern "C" __declspec(dllexport) void my_get_foreground_window()
{
	char buf[100];
	HWND hWnd;
	hWnd = GetForegroundWindow();
	GetWindowTextA(hWnd, buf, 100);
	key_log_info->SetWinTitle(buf);
	key_log_info->SetWindowHandle(hWnd);
}

extern "C" __declspec(dllexport) void my_set_process_id()
{
	HWND h = key_log_info->GetWindowHandle();
	DWORD id;
	GetWindowThreadProcessId(h, &id);
	key_log_info->SetProcessId(id);
}

extern "C" __declspec(dllexport) DWORD get_process_id()
{
	return key_log_info->GetProcessId();
}

extern "C" __declspec(dllexport) char* get_win_title()
{
	return key_log_info->GetWinTitle();
}

extern "C" __declspec(dllexport) void say_hey()
{
	Sayhey();
}


