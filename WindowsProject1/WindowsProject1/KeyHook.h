#pragma once
#ifdef KEYHOOK_EXPORTS
#define KEYHOOK __declspec(dllexport)
#else
#define KEYHOOK __declspec(dllimport)
#endif // KEYHOOK_EXPORTS

#include <Windows.h>
#include <string>
#include <stdio.h>

//KEYHOOK LRESULT CALLBACK HookProc(int, WPARAM, LPARAM);
//KEYHOOK BOOL StartHook();
//KEYHOOK BOOL EndHook();

LRESULT CALLBACK HookProc(int, WPARAM, LPARAM);
BOOL StartHook(HWND);
BOOL EndHook();
VOID SetStr();
LPCWSTR GetStr();
LPCWSTR GetStrr(std::string);
