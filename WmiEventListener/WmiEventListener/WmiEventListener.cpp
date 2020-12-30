// WmiEventListener.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "WmiEventListener.h"

#include <wbemidl.h>

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
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
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

HWND g_hwndListBox = NULL;

class CObjectSink :public IWbemObjectSink 
{
public:
    // STDMETHODIMP: HRESULT STDMETHODCALLTYPE
    // HRESULT : long , 成功か失敗かだけでなく失敗した場合の理由も示す
    // https://www.usefullcode.net/2007/03/hresult.html
    // STDMETHODCALLTYPE : __stdcall 呼び出し規約
    // https://www.keicode.com/winprimer/wp07b.php
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    STDMETHODIMP Indicate(LONG IObjectCount, IWbemClassObject** ppObjArray);
    STDMETHODIMP SetStatus(long IFlags, HRESULT hResult, BSTR strParam, IWbemClassObject* pObjParam);

    CObjectSink();
private:
    LONG m_cRef;
};

/// <summary>
/// あるCOMオブジェクトがサポートしているCOMインターフェイスを取得
/// COMオブジェクト:
/// Component Object Model(COM)は、便利な機能を提供するプログラム
/// このプログラム実行時にはメモリ上に実体が生成される
/// 生成された実体のことをCOMオブジェクトという
/// COMインターフェイス: COMオブジェクトを外部から利用するためのアクセス箇所
/// </summary>
/// <param name="riid">インターフェイスのGUID</param>
/// <param name="ppvObject">オブジェクトのポインタのポインタ</param>
/// <returns></returns>
STDMETHODIMP CObjectSink::QueryInterface(REFIID riid, void** ppvObject)
{
    *ppvObject = NULL;

    // 指定したIIDとCOMオブジェクトが持つCOMインターフェイスIIDが一致するか確認
    if (IsEqualIID(riid, IID_IUnknown)) { *ppvObject = this; }
    else { return E_NOINTERFACE; }

    AddRef();

    return S_OK;
}

/// <summary>
/// 参照カウンタを更新する
/// 複数スレッドでオブジェクトを操作する場合、同時に
/// 変数を使用することを防ぐため、InterlockedIncrement()を使う
/// </summary>
/// <returns></returns>
STDMETHODIMP_(ULONG) CObjectSink::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

/// <summary>
/// COMオブジェクトの開放・破棄を行う
/// </summary>
/// <returns></returns>
STDMETHODIMP_(ULONG) CObjectSink::Release()
{
    if (InterlockedDecrement(&m_cRef) == 0)
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

/// <summary>
/// イベントが発生した場合に呼ばれる関数
/// </summary>
/// <param name="IObjectCount"></param>
/// <param name="ppObjArray"></param>
/// <returns></returns>
STDMETHODIMP CObjectSink::Indicate(LONG IObjectCount, IWbemClassObject **ppObjArray)
{
    HRESULT hr;
    VARIANT var; // 汎用型
    BSTR bstr; //WCHAR
    IWbemClassObject* pObject;

    // プロパティ"Caption"を指定することでプロセス名を取得するためには、
    // 一度、"TargetInstance"を指定したりして準備する必要がある(作法?)
    // https://stackoverflow.com/questions/31753518/get-process-handle-of-created-processes-windows
    bstr = SysAllocString(L"TargetInstance"); // BSTRの初期化
    ppObjArray[0]->Get(bstr, 0, &var, 0, 0); // 
    // COMオブジェクトを取得
    var.pdispVal->QueryInterface(IID_PPV_ARGS(&pObject));
    VariantClear(&var);
    // プロセス名を取得
    pObject->Get(L"Caption", 0, &var, 0, 0);

    SendMessage(g_hwndListBox, LB_ADDSTRING, 0, (LPARAM)var.bstrVal);

    // 開放
    SysFreeString(bstr); // BSTR
    pObject->Release(); // COMオブジェクト
    VariantClear(&var); // VARIANT

    return WBEM_S_NO_ERROR;
}

