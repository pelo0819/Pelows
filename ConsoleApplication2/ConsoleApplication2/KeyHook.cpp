#include "KeyHook.h"
#include <Windows.h>
#include <iostream>

HHOOK hHook = NULL;

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
    {
        return CallNextHookEx(hHook, nCode, wParam, lParam);
    }
    //if (nCode == HC_ACTION) {

    //    //正数を返すと、キーボードが無効になる
    //    return 1;

    //}
    std::cout << "HookProc()\n";
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

BOOL StartHook()
{
    std::cout << "StartHook()\n";
    HINSTANCE hInst;
    hInst = (HINSTANCE)GetWindowLong(, GWL_HINSTANCE);
    hHook = SetWindowsHookEx(WH_KEYBOARD, HookProc, hInst, 0);
    return g_hHook != NULL;
}

BOOL EndHook()
{
    std::cout << "EndHook()\n";
    return UnhookWindowsHookEx(hHook);
}
