// ActiveXTest2.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "ActiveXTest2.h"
#include "BrowserWindow.h"

#include "atlbase.h"
#include "atlcom.h"
#include "atlhost.h"

#define MAX_URL MAX_PATH
#define MAX_POST (2000)

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void CreateATXWindow(HINSTANCE hInst, HWND hWnd);
static HWND s_hWebAccessDlg = NULL;
static CComModule _Module;
static CComQIPtr<IWebBrowser2> pWB2;
static void InitActiveX(HINSTANCE hInst);
static BOOL CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static char s_szURL[MAX_URL];
static char s_szPOST[MAX_POST];

void tryLaunchWindow(HINSTANCE hInstance, int nCmdShow, HWND hWnd);

WCHAR s_title[] = { 0 };
HWND hParent;
int cmdShow = 0;

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
    LoadStringW(hInstance, IDC_ACTIVEXTEST2, szWindowClass, MAX_LOADSTRING);

	cmdShow = nCmdShow;

    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ACTIVEXTEST2));

	InitActiveX(hInstance);

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ACTIVEXTEST2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ACTIVEXTEST2);
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

   hParent = hWnd;

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
                //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
#if true
				CreateATXWindow(hInst, hWnd);
#else
				BrowserWindow::RegisterClassW(hInst);
				tryLaunchWindow(hInst, cmdShow, hWnd);
#endif
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

static VOID InitActiveX(HINSTANCE hInst)
{
	CoInitialize(NULL);
	_Module.Init(0, hInst, &LIBID_ATLLib);

	if (!AtlAxWinInit())
	{
		//MessageBox(NULL, TEXT("atcitveX init fail"), TEXT("atcitveX init fail"), MB_OK);
	}
	else
	{
		//MessageBox(NULL, TEXT("atcitveX init succ"), TEXT("atcitveX init succ"), MB_OK);
	}
}

void CreateATXWindow(HINSTANCE hInst, HWND hWnd)
{
	HWND hDlg = ATL::AtlAxCreateDialog(hInst, MAKEINTRESOURCE(IDD_FORMVIEW), hWnd, DialogProc, 0);
	//HWND hDlg = ATL::AtlAxCreateDialog(hInst, MAKEINTRESOURCE(IDD_PROPPAGE_LARGE), hWnd, DialogProc, 0);
	/*
	if (hDlg != NULL)
	{
		MessageBox(NULL, TEXT("succ"), TEXT("succ"), MB_OK);
	}
	else
	{
		MessageBox(NULL, TEXT("fail"), TEXT("fail"), MB_OK);
	}
	*/
	s_hWebAccessDlg = hDlg;
	
	ShowWindow(hDlg, cmdShow);
	UpdateWindow(hDlg);
}

static BOOL CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_INITDIALOG:
	{
		strcpy_s(s_szURL, "https://www.bing.com/");
		MessageBox(NULL, TEXT("init"), TEXT("init"), MB_OK);
		InvalidateRect(hWnd, NULL, TRUE);


#if true
		CComPtr<IUnknown> punkIE;
		LPOLESTR pProgID = NULL;
		CHAR strProgID[256];
		ProgIDFromCLSID(CLSID_WebBrowser, &pProgID);
		WideCharToMultiByte(CP_ACP, 0, pProgID, -1, strProgID, sizeof(strProgID), NULL, NULL);
		CoTaskMemFree(pProgID);

#
		HWND hWndIE = CreateWindowEx(
			0, 
			L"AtlAxWin140", 
			L"Shell.Explorer.2", 
			WS_CHILD | WS_VISIBLE, 
			CW_USEDEFAULT, 
			0, 
			500, 
			300, 
			hWnd, 
			reinterpret_cast<HMENU>(0), 
			hInst, 
			0);

		if (ATL::AtlAxGetControl(hWndIE, &punkIE) == S_OK)
		{
			pWB2 = punkIE;
			if (pWB2)
			{
				MessageBox(NULL, TEXT("succ ie"), TEXT("succ ie"), MB_OK);

				VARIANT vpost;
				VariantInit(&vpost);
				vpost.vt = VT_ARRAY | VT_UI1;

				CComVariant ve;
				CComVariant vurl(s_szURL);
				CComVariant vheader("Content-Type: application/x-www-form-urlencode\r\n");
				CComVariant vempty;
				CComVariant TargetFrameName("_top");
				pWB2->Navigate2(&vurl, &vempty, &TargetFrameName, &vpost, &vheader);

			}
			else
			{
				MessageBox(NULL, TEXT("fail ie"), TEXT("fail ie"), MB_OK);
			}
		}
		else
		{
			MessageBox(NULL, TEXT("fail getcontroll"), TEXT("fail getcontroll"), MB_OK);
		}

		if (hWndIE)
		{
			MessageBox(hWndIE, TEXT("show ie window"), TEXT("show ie window"), MB_OK);
			//ShowWindow(hWndIE, SW_SHOW);
			//UpdateWindow(hWndIE);
		}
#else
		BrowserWindow::RegisterClassW(hInst);
		tryLaunchWindow(hInst, SW_SHOW, hWnd);
#endif
	}
		break;
	case WM_ACTIVATE:
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return true;

	case WM_DESTROY:
		break;
	}
	return false;
}


void tryLaunchWindow(HINSTANCE hInstance, int nCmdShow, HWND hWnd)
{
	BOOL launched = BrowserWindow::LaunchWindow(hInstance, nCmdShow, hWnd);
	if (!launched)
	{
		int msgboxID = MessageBox(NULL, L"Could not launch the browser", L"Error", MB_RETRYCANCEL);

		switch (msgboxID)
		{
		case IDRETRY:
			tryLaunchWindow(hInstance, nCmdShow, hWnd);
			break;
		case IDCANCEL:
		default:
			PostQuitMessage(0);
		}
	}
	else
	{
		MessageBox(NULL, TEXT("show ie window"), TEXT("show ie window"), MB_OK);
	}
}