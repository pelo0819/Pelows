#include "pch.h"
#include "Pell.h"
#include "window_manager.h"
#include "keyboard_tracer.h"

#pragma data_seg(".sharedata")
HHOOK hHook = 0;
#pragma data_seg()
HWND m_hWnd = 0;
HWND hWnd = 0;
DWORD m_thread_id = 0;
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
		hInst = (HINSTANCE)hModule;
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
	delete tracer;
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
	m_thread_id = GetWindowThreadProcessId(m_hWnd, NULL);
}
//
//extern "C" __declspec(dllexport) LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
//{
//	std::cout << "hello" << std::endl;
//	tracer->registKeyLog(wParam);
//	//MessageBoxW(m_hWnd, L"hello", TEXT("click"), MB_OK);
//	if (nCode < 0)
//	{ 
//		return CallNextHookEx(hHook, nCode, wParam, lParam);
//	}
//	tracer->setLpBinary(lParam);
//	if (tracer->getLpBinary(0) == 1)
//	{
//		tracer->registKeyLog(wParam);
//		tracer->print_log();
//	}
//	return CallNextHookEx(hHook, nCode, wParam, lParam);
//}


LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		return CallNextHookEx(0, nCode, wParam, lParam);
	}
	//std::cout << "hello" << std::endl;
	//tracer->registKeyLog(wParam);
	//MessageBoxW(m_hWnd, L"hello", TEXT("click"), MB_OK);
	MessageBoxW(hWnd, L"hello", TEXT("click"), MB_OK);

	//SendMessage(m_hWnd, WM_KEYDOWN, wParam , lParam);
	//PostThreadMessage(m_thread_id, WH_KEYBOARD, wParam, lParam);

	/*tracer->setLpBinary(lParam);
	if (tracer->getLpBinary(0) == 1)
	{
		tracer->registKeyLog(wParam);
		tracer->print_log();
	}*/
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}
extern "C" __declspec(dllexport) void StartHook()
{
	if (hWnd == NULL)
	{
		std::cout << "[!!!] window handle is null. fail to start hook" << std::endl;
		return;
	}
	
	hHook = SetWindowsHookEx(WH_KEYBOARD, HookProc, hInst, 0);
	
	//ターゲットが強制終了版
	// hHook = SetWindowsHookEx(WH_KEYBOARD, HookProc, hInst, GetWindowThreadProcessId(hWnd, NULL));

	//hHook = SetWindowsHookEx(WH_KEYBOARD, HookProc, hInst, GetCurrentThreadId());
	
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
	std::cout << "wait" << std::endl;
	MSG msg;

	while (GetMessage(&msg, m_hWnd, 0, 0) > 0)
	{
		std::cout << "get msg" << std::endl;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}



	/*while ((bValue = GetMessage(&msg, NULL, 0, 0)) != 0)
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

	/*while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			std::cout << "get PM_REMOVE" << std::endl;
			if (msg.message == WM_QUIT) {
				DispatchMessage(&msg);
				break;
			}
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
	}*/
}



