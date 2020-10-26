#include "pch.h"
#include "Pell.h"
#include "window_manager.h"
#include "keyboard_tracer.h"
#include <thread>

#pragma data_seg(".sharedata")
HHOOK hHook = 0;
bool isEnter = false;
#pragma data_seg()
HWND m_hWnd = 0;
HWND hWnd = 0;
DWORD m_thread_id = 0;
HINSTANCE hInst;
WindowManager* window_manager;
KeyboardTracer* tracer;
HANDLE hThread;
BOOL isObserve = true;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hInst = (HINSTANCE)hModule;
		hThread = CreateThread(NULL, 0, Thread, (LPVOID)NULL, 0, &m_thread_id);
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
	//hWnd = GetForegroundWindow();
	hWnd = FindWindow(TEXT("Notepad"), NULL);
	if (hWnd == NULL)
	{
		std::cout << "[!!!] fail to set Window handle." << std::endl;

	}
	else
	{
		std::cout << "[*] set Window handle." << std::endl;
	}
	char buf[100];
	GetWindowTextA(hWnd, buf, 100);
	window_manager->SetWinTitle(buf);
	window_manager->SetWindowHandle(hWnd);
}

VOID MyMessageBox(std::string str)
{
	int len = strlen(str.c_str());
	wchar_t t[128] = L"";
	for (int i = 0; i < len; i++)
	{
		t[i] = str[i];
	}
	MessageBoxW(m_hWnd, t, TEXT("click"), MB_OK);
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
	//m_hWnd = FindWindow(TEXT("Windows PowerShell"), NULL);
	m_hWnd = FindWindow(NULL, TEXT("Windows PowerShell"));
	if (m_hWnd == NULL)
	{
		std::cout << "nothing window" << std::endl;
	}
	else
	{
		//m_thread_id = GetWindowThreadProcessId(m_hWnd, NULL);
	}

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
	
	/*if (hWnd == NULL)
	{
		char buf[100];
		GetWindowTextA(hWnd, buf, 100);
		MyMessageBox(buf);
	}*/
	//SendMessage(m_hWnd, WM_KEYDOWN, wParam , lParam);
	//PostThreadMessage(m_thread_id, WM_NULL, wParam, lParam);
	isEnter = true;

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

	if (hHook == NULL)
	{
		std::cout << "[!!!] hook is null. fail to start hook" << std::endl;
		return;
	}

	std::cout << "[*] successfull, start hook." << std::endl;
	isObserve = true;
	return;
}

extern "C" __declspec(dllexport) bool EndHook()
{
	isObserve = false;
	tracer->print_log();
	CloseHandle(hThread);
	return UnhookWindowsHookEx(hHook);
}

extern "C" __declspec(dllexport) void Wait()
{
	std::cout << "wait" << std::endl;
	MSG msg;

	/*while (GetMessage(&msg, NULL, 0, 0))
	{
		std::cout << "get msg" << std::endl;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}*/

	while (isObserve)
	{
		if (isEnter)
		{
			std::cout << "get enter" << std::endl;
			isEnter = false;
		}
	}
	std::cout << "waitend" << std::endl;

}

DWORD WINAPI Thread(LPVOID pData)
{
	std::cout << "thread" << std::endl;
	Wait();
	return 0;
}


