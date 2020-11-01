#include "KeyHook.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <stdio.h>

HWND m_hWnd;
HHOOK hHook;
int callbackCount = 0;
LPCWSTR lpszStr = TEXT("helllllo");
std::string key_log = "";


LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
    {
        return CallNextHookEx(hHook, nCode, wParam, lParam);
    }
    int fUp = 0x40000000 & lParam;
    TCHAR tcStr[128];
    const int n = 32;
    int bin[n] = {};
    Binary(bin, lParam);

    std::string str = "";
    int stloke = 0;
    for (int i = 0; i < 16; i++)
    {
        stloke += bin[i];
    }

    if (nCode == HC_ACTION && !fUp)
    {
        RegistKeyLog(wParam);
        MyMessageBox(key_log);
    }
    
    /*if (bin[0] == 1 && bin[29] == 0)
    {
        key_log = std::to_string(stloke);
        RegistKeyLog(wParam);
        MyMessageBox(key_log);
    }*/
    callbackCount++;
    SetStr();
    InvalidateRect(m_hWnd, NULL, TRUE);

    return TRUE;
}

void RegistKeyLog(WPARAM virkey)
{
    key_log += Keycord(virkey);
}


void Binary(int *ret, LPARAM lparam)
{
    for (int i = 0; lparam > 0; i++)
    {
        ret[i] = lparam % 2;
        lparam = lparam / 2;
    }
}

BOOL StartHook(HWND hWnd)
{
    m_hWnd = hWnd;
    HINSTANCE hInst;
    hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
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
    wchar_t t[256] = L"";
    for (int i = 0; i < len; i++) 
    {
        t[i] = str[i];
    }
    DrawText(hdc, t, -1, rc, format);
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

int GetCallbackCnt() { return callbackCount; }

std::string Keycord(WPARAM virkey)
{
    std::string ret = "";
    switch (virkey)
    {
    case 0x08:
        ret += "B";
        break;
    case 0x0d://enter
        key_log.clear();
        break;
    case 0x20:
        ret += " ";
        break;
    case 0x25:// left
        ret += "L";
        break;
    case 0x26:
        ret += "U";
        break;
    case 0x27:
        ret += "R";
        break;
    case 0x28:
        ret += "D";
        break;
    case 0x2e://clear
        ret += "C";
        break;
    case 0x30:
        ret += "0";
        break;
    case 0x31:
        ret += "1";
        break;
    case 0x32:
        ret += "2";
        break;
    case 0x33:
        ret += "3";
        break;
    case 0x34:
        ret += "4";
        break;
    case 0x35:
        ret += "5";
        break;
    case 0x36:
        ret += "6";
        break;
    case 0x37:
        ret += "7";
        break;
    case 0x38:
        ret += "8";
        break;
    case 0x39:
        ret += "9";
        break;
    case 0x41:
        ret += "a";
        break;
    case 0x42:
        ret += "b";
        break;
    case 0x43:
        ret += "c";
        break;
    case 0x44:
        ret += "d";
        break;
    case 0x45:
        ret += "e";
        break;
    case 0x46:
        ret += "f";
        break;
    case 0x47:
        ret += "g";
        break;
    case 0x48:
        ret += "h";
        break;
    case 0x49:
        ret += "i";
        break;
    case 0x4a:
        ret += "j";
        break;
    case 0x4b:
        ret += "k";
        break;
    case 0x4c:
        ret += "l";
        break;
    case 0x4d:
        ret += "m";
        break;
    case 0x4e:
        ret += "n";
        break;
    case 0x4f:
        ret += "o";
        break;
    case 0x50:
        ret += "p";
        break;
    case 0x51:
        ret += "q";
        break;
    case 0x52:
        ret += "r";
        break;
    case 0x53:
        ret += "s";
        break;
    case 0x54:
        ret += "t";
        break;
    case 0x55:
        ret += "u";
        break;
    case 0x56:
        ret += "v";
        break;
    case 0x57:
        ret += "w";
        break;
    case 0x58:
        ret += "x";
        break;
    case 0x59:
        ret += "y";
        break;
    case 0x5a:
        ret += "z";
        break;
    case 0xba:
        ret += ":";
        break;
    case 0xbb:
        ret += ";";
        break;
    case 0xbc:
        ret += ",";
        break;
    case 0xbd:
        ret += "-";
        break;
    case 0xbe:
        ret += ".";
        break;
    case 0xbf:
        ret += "/";
        break;
    case 0xc0:
        ret += "@";
        break;
    case 0xdb:
        ret += "[";
        break;
    case 0xdc:
        ret += "\\";
        break;
    case 0xdd:
        ret += "]";
        break;
    case 0xde:
        ret += "^";
        break;
    default:
        ret += "?";
        break;
    }
    return ret;
}




