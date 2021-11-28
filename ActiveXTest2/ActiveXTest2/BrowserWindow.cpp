
#include "BrowserWindow.h"

#include "shlobj.h"
#include <Urlmon.h>
#pragma comment (lib, "Urlmon.lib")

using namespace Microsoft::WRL;

WCHAR BrowserWindow::s_windowClass[] = { 0 };
WCHAR BrowserWindow::s_title[] = { 0 };

ATOM BrowserWindow::RegisterClass(_In_ HINSTANCE hInstance)
{
	//LoadStringW(hInstance, IDS_APP_TITLE, s_title, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_ACTIVEXTEST2, s_windowClass, MAX_LOADSTRING);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProcStatic;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL; // LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WEBVIEWTEST));
	wcex.hCursor = NULL;// LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL; //MAKEINTRESOURCEW(IDC_WEBVIEWTEST2);
	wcex.lpszClassName = L"BrowserWindow";// s_windowClass;
	wcex.hIconSm = NULL;// LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL BrowserWindow::LaunchWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow, HWND hWnd)
{
	BrowserWindow* window = new BrowserWindow();
	if (!window->InitInstance(hInstance, nCmdShow, hWnd))
	{
		delete window;
		return FALSE;
	}
	return TRUE;
}

BOOL BrowserWindow::InitInstance(HINSTANCE hInstance, int nCmdShow, HWND hWnd)
{
	//m_hWnd = hWnd;
	m_hInst = hInstance; // Store app instance handle
	m_parent = hWnd;
	LoadStringW(m_hInst, IDS_APP_TITLE, s_title, MAX_LOADSTRING);

	m_hWnd = CreateWindowW(
		L"AtlAxWin140",
		L"Shell.Explorer.2",
		WS_CHILD | WS_VISIBLE,//WS_OVERLAPPEDWINDOW, //WS_CHILD | WS_VISIBLE,
		0,
		0,
		500,
		300,
		m_parent,
		nullptr,
		m_hInst,
		nullptr
	);

	if (!m_hWnd)
	{
		MessageBox(NULL, TEXT("failed"), TEXT("failed"), MB_OK);
		return FALSE;
	}
	else
	{
		//MessageBox(NULL, TEXT("success"), TEXT("success"), MB_OK);
	}

	// Make the BrowserWindow instance ptr available through the hWnd

	ShowWindow(m_hWnd, nCmdShow);
	UpdateWindow(m_hWnd);
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT
			{

				// Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
				env->CreateCoreWebView2Controller(m_hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[this](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
						//MessageBox(NULL, TEXT("fail"), TEXT("controller is null"), MB_OK);
						if (controller != nullptr) {
							m_controlsController = controller;
							m_controlsController->get_CoreWebView2(&m_controlsWebView);
							OutputDebugString(L"controller is all\n");
						}
						// Add a few settings for the webview
						// The demo step is redundant since the values are the default settings
						ICoreWebView2Settings* Settings;
						m_controlsWebView->get_Settings(&Settings);
						Settings->put_IsScriptEnabled(TRUE);
						Settings->put_AreDefaultScriptDialogsEnabled(TRUE);
						Settings->put_IsWebMessageEnabled(TRUE);

						// Resize WebView to fit the bounds of the parent window
						RECT bounds;
						GetClientRect(m_hWnd, &bounds);
						m_controlsController->put_Bounds(bounds);

						// Schedule an async task to navigate to Bing
						m_controlsWebView->Navigate(L"https://www.bing.com/");

						return S_OK;
					}).Get());
				return S_OK;
			}).Get());

	if (!SUCCEEDED(hr))
	{
		OutputDebugString(L"Content WebViews environment creation failed\n");
		return FALSE;
	}
	else
	{
		MessageBox(NULL, TEXT("hello edge"), TEXT("hello edge"), MB_OK);
	}


	return TRUE;
}

LRESULT CALLBACK BrowserWindow::WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BrowserWindow* browser_window = reinterpret_cast<BrowserWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (browser_window != nullptr)
	{
		return browser_window->WndProc(hWnd, message, wParam, lParam);  // Forward message to instance-aware WndProc
	}
	else
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK BrowserWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* minmax = reinterpret_cast<MINMAXINFO*>(lParam);
		minmax->ptMinTrackSize.x = m_minWindowWidth;
		minmax->ptMinTrackSize.y = m_minWindowHeight;
		break;
	}
	case WM_DPICHANGED:
	{
	}
	case WM_SIZE:
	{
	}
	break;
	case WM_CLOSE:
	{
	}
	break;
	case WM_NCDESTROY:
	{
		SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
		delete this;
		PostQuitMessage(0);
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	default:
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
	}
	return 0;
}




