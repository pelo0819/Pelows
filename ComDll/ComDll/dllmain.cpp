// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"

#include <wbemidl.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "Ole32.lib")

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

static IDispatch* pDispatch = NULL;
static DISPID dispid;
static wchar_t* methodName = new wchar_t[MAX_LEN];
static bool initialized = false;

/// <summary>
/// 初期化
/// </summary>
/// <returns></returns>
extern "C" __declspec(dllexport) int Initialize()
{
    if (initialized) { return 0; }

    HRESULT hr;
    // COMを利用できるようにする
    hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        printf("[!!!] failed CoInitializeEx()\n");
        return 1;
    }
    else
    {
        printf("[*] success CoInitializeEx()\n");
        initialized = true;
        return 0;
    }
}

/// <summary>
/// インスタンスの生成
/// </summary>
/// <returns></returns>
extern "C" __declspec(dllexport) int CreateInstance(const char* name)
{
    HRESULT hr;
    CLSID clsId;
#if false
    char buffer[MAX_LEN];
    printf("Input COM object name ...\n");
    printf(">>>");
    if (fgets(buffer, MAX_LEN, stdin) == NULL)
    {
        return 1;
    }
    buffer[strlen(buffer) - 1] = '\0';*/
#endif
    wchar_t* progId = new wchar_t[MAX_LEN];
    size_t size = 0;
    mbstowcs_s(&size, progId, MAX_LEN, name, _TRUNCATE);

    hr = CLSIDFromProgIDEx(progId, &clsId);
    if (FAILED(hr))
    {
        printf("[!!!] failed CLSIDFromProgIDEx() %s\n", name);
        return 1;
    }
    else
    {
        printf("[*] success CLSIDFromProgIDEx() %s\n", name);
    }

    hr = CoCreateInstance(
        clsId,
        0,
        CLSCTX_LOCAL_SERVER,
        IID_IDispatch,
        reinterpret_cast<LPVOID*>(&pDispatch));

    if (FAILED(hr))
    {
        printf("[!!!] failed CoCreateInstance() %s\n", name);
        return 1;
    }
    else
    {
        printf("[*] success CoCreateInstance() %s\n", name);
    }

    return 0;
}

/// <summary>
/// メソッドを設定
/// </summary>
/// <returns></returns>
extern "C" __declspec(dllexport) int SetMethod(const char* n)
{
#if false
    char buffer[MAX_LEN];
    wchar_t* c = new wchar_t[MAX_LEN];
    printf("Input method name ...\n");
    printf(">>>");
    if (fgets(buffer, MAX_LEN, stdin) == NULL)
    {
        return 1;
    }
    buffer[strlen(buffer) - 1] = '\0';
#endif

    size_t size = 0;
    mbstowcs_s(&size, methodName, MAX_LEN, n, _TRUNCATE);
    return 0;
}

/// <summary>
/// セットしたメソッドの実行
/// </summary>
/// <returns></returns>
extern "C" __declspec(dllexport) int Invoke()
{
    HRESULT hr;
    DISPPARAMS params = { NULL, NULL, 0, 0 };
    OLECHAR* szMember = methodName;

    hr = pDispatch->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid);
    if (FAILED(hr))
    {
        printf("[!!!] failed GetIDsOfNames() %ls\n", methodName);
        return 1;
    }
    else
    {
        printf("[*] success GetIDsOfNames() %ls\n", methodName);
    }

    hr = pDispatch->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
    if (FAILED(hr))
    {
        printf("[!!!] failed Invoke() %ls\n", methodName);
        return 1;
    }
    else
    {
        printf("[*] success Invoke() %ls\n", methodName);
    }

    if (pDispatch != NULL) { pDispatch->Release(); }

    return 0;
}

/// <summary>
/// 終了処理
/// </summary>
/// <returns></returns>
extern "C" __declspec(dllexport) int End()
{
    if (!initialized) { return 0; }
    // COMの後始末
    CoUninitialize();
    printf("[*] end\n");
    return 0;
}


