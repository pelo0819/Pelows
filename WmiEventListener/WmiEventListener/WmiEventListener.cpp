// WmiEventListener.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "WmiEventListener.h"

#include <wbemidl.h>
#include "CObjectSink.h"
#include "Params.h"

#pragma comment (lib, "wbemuuid.lib")

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//HWND g_hwndListBox = NULL;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WMIEVENTLISTENER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WMIEVENTLISTENER));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WMIEVENTLISTENER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WMIEVENTLISTENER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static IWbemServices* pNamespace = NULL;
    static IWbemObjectSink* pStubSink = NULL;

    switch (message)
    {
    case WM_CREATE:
        HRESULT hr;
        BSTR bstrNamespace;
        BSTR bstrQuery;
        BSTR bstrLanguage;
        IWbemLocator* pLocator;
        IUnsecuredApartment* pUnsecApp;
        IUnknown* pStubUnk;
        CObjectSink* pObjectSink;


        // アパートメントをMTAに設定
        // https://www.kekyo.net/2013/07/22/382
        CoInitializeEx(0, COINIT_MULTITHREADED);

        // WMIはDCOM(COMの改良版)を使用している。DCOMはRPCを使用していて、
        // RPCの通信にはセキュリティ情報を指定する必要がある
        CoInitializeSecurity(
            NULL,
            -1,
            NULL,
            NULL,
            RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE,
            NULL);

        // COMのインスタンスを作成
        CoCreateInstance(
            CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&pLocator));
        
        // 名前空間に接続してIWbemServices(pNamespace)を取得
        bstrNamespace = SysAllocString(L"root\\cimv2");
        hr = pLocator->ConnectServer(
            bstrNamespace, 
            NULL, 
            NULL, 
            NULL, 
            0, 
            NULL, 
            NULL, 
            &pNamespace);

        if (FAILED(hr))
        {
            SysFreeString(bstrNamespace);
            pLocator->Release();
            return -1;
        }

        // pNameSpaceに個別の設定を行う(MSDNのサンプル)
        hr = CoSetProxyBlanket(
            pNamespace, 
            RPC_C_AUTHN_WINNT, 
            RPC_C_AUTHZ_NONE, 
            NULL, 
            RPC_C_AUTHN_LEVEL_CALL, 
            RPC_C_IMP_LEVEL_IMPERSONATE, 
            NULL, 
            EOAC_NONE);

        hr = CoCreateInstance(
            CLSID_UnsecuredApartment, 
            NULL, 
            CLSCTX_LOCAL_SERVER, 
            IID_PPV_ARGS(&pUnsecApp));

        pObjectSink = new CObjectSink;
        pUnsecApp->CreateObjectStub(pObjectSink, &pStubUnk);
        pStubUnk->QueryInterface(IID_IWbemObjectSink, (void**)&pStubSink);

        //bstrQuery = SysAllocString(L"SELECT * FROM __InstanceCreationEvent WITHIN 1 WHERE TargetInstance ISA 'Win32_Process'");
        bstrQuery = SysAllocString(L"SELECT * FROM __InstanceCreationEvent WITHIN 0.1 WHERE TargetInstance ISA 'CIM_CreateDirectoryAction'");
        bstrLanguage = SysAllocString(L"WQL");

        hr =pNamespace->ExecNotificationQueryAsync(
            bstrLanguage,
            bstrQuery,
            WBEM_FLAG_SEND_STATUS,
            NULL,
            pStubSink);

        if (FAILED(hr))
        {
            MessageBox(NULL, TEXT("[OK] が押されました。"),
                TEXT("結果"), MB_ICONINFORMATION);
        }

        SysFreeString(bstrLanguage);
        SysFreeString(bstrQuery);
        SysFreeString(bstrNamespace);
        pObjectSink->Release();
        pStubUnk->Release();
        pUnsecApp->Release();
        pLocator->Release();


        Params::g_hwndListBox = CreateWindowEx(0, TEXT("LISTBOX"), NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        break;
    case WM_SIZE:
        MoveWindow(Params::g_hwndListBox, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;
    case WM_DESTROY:
        if (pNamespace != NULL) {
            if (pStubSink != NULL) {
                pNamespace->CancelAsyncCall(pStubSink);
                pStubSink->Release();
            }
            pNamespace->Release();
        }
        CoUninitialize();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


