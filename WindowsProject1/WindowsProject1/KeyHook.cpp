#include "KeyHook.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <stdio.h>

HWND m_hWnd;
HHOOK hHook;
int callbackCount = 0;
LPCWSTR lpszStr = TEXT("helllllo");


LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
    {
        return CallNextHookEx(hHook, nCode, wParam, lParam);
    }
    if (wParam >= 0x30 && wParam <= 0x39) {
        return CallNextHookEx(hHook, nCode, wParam, lParam);
    }
    callbackCount++;
    SetStr();
    InvalidateRect(m_hWnd, NULL, TRUE);
    return TRUE;
}

BOOL StartHook(HWND hWnd)
{
    m_hWnd = hWnd;
    HINSTANCE hInst;
    hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    //hHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)HookProc, hInst, 0);
    hHook = SetWindowsHookEx(WH_KEYBOARD, HookProc, hInst, GetCurrentThreadId());
    if (hHook == NULL)
    {
        lpszStr = TEXT("Fail starting hook.");
        return FALSE;
    }
    else 
    {
        lpszStr = TEXT("Successful starting hook.");
        return TRUE;
    }
}

BOOL EndHook()
{
    return UnhookWindowsHookEx(hHook);
}

VOID SetStr() 
{
    lpszStr = TEXT("callbackCnt:%d");
}


VOID MyDrawText(HDC hdc, std::string str, LPRECT rc, UINT format)
{
    int len = strlen(str.c_str());
    wchar_t t[128] = L"";
    for (int i = 0; i < len; i++) 
    {
        t[i] = str[i];
    }
    DrawText(hdc, t, -1, rc, format);
}

int GetCallbackCnt() { return callbackCount; }



