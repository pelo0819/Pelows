
#include "BrowserWindow.h"

#include "shlobj.h"
#include <Urlmon.h>
#pragma comment (lib, "Urlmon.lib")

using namespace Microsoft::WRL;

extern BrowserWindow *browserWindow;
//static constexpr LPCWSTR s_subFolder = "C:\\Users\\81801\\Desktop\\webview2\\Microsoft.WebView2.FixedVersionRuntime.92.0.902.78.x86\\";

LRESULT CALLBACK WndProcBrowserWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		if (browserWindow) browserWindow->ProcOnPaint();
		break;
	case WM_DESTROY:
		if (browserWindow) browserWindow->ProcOnDestroy();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

ATOM BrowserWindow::RegisterClass(_In_ HINSTANCE hInstance)
{
	m_hInst = hInstance;

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProcBrowserWindow;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = nullptr;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"BrowserWindow";
	wcex.hIconSm = nullptr;

	return RegisterClassExW(&wcex);
}

HWND BrowserWindow::CreateBrowserWindow()
{
	m_hWnd = CreateWindowW(
		L"BrowserWindow",
		L"WebView",
		WS_CHILD | WS_VISIBLE,
		m_posX,
		m_posY,
		m_width,
		m_height,
		m_hWndParent,
		nullptr,
		m_hInst,
		nullptr
	);
	return m_hWnd;
}

bool BrowserWindow::LaunchWindow()
{
	if (!SetupWebView2())
	{
		delete this;
		return FALSE;
	}
	return TRUE;
}

void BrowserWindow::SetParentWindow(HWND hParent)
{
	m_hWndParent = hParent;
}

bool BrowserWindow::SetupWebView2()
{
	if (!m_hWnd)
	{
		MessageBox(m_hWnd, TEXT("fail"), TEXT("fail"), MB_OK);
	}

	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	
	char current[512];
	GetCurrentDirectory(511, current);
	//MessageBox(NULL, current, TEXT("current"), MB_OK);
	std::string path = current;
	path.append("\\Microsoft.WebView2.FixedVersionRuntime.92.0.902.78.x86\\");
	//MessageBox(NULL, path.c_str(), TEXT("current"), MB_OK);

	//std::string path2 = "C:/Users/81801/source/repos/Webview2Test/Microsoft.WebView2.FixedVersionRuntime.92.0.902.78.x86";
	//std::string path2 = "C:\\Users\\81801\\source\\repos\\Webview2Test\\Microsoft.WebView2.FixedVersionRuntime.92.0.902.78.x86\\";
	std::string path2 = "C:\\Users\\81801\\Desktop\\webview2\\Microsoft.WebView2.FixedVersionRuntime.92.0.902.78.x86\\";
	//std::string path2 = ".\\Microsoft.WebView2.FixedVersionRuntime.92.0.902.78.x86";


	size_t s = 0;
	int len = path2.size() + 1;
	wchar_t *wpath = new wchar_t[len];
	mbstowcs_s(&s, wpath, len, path2.c_str(), _TRUNCATE);

	HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(L"C:\\Users\\81801\\Desktop\\webview2\\Microsoft.WebView2.FixedVersionRuntime.92.0.902.78.x86\\", nullptr, nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[this](HRESULT result, ICoreWebView2Environment *env) -> HRESULT
			{
				return CallbackOnCompleteWebView2Enviornment(env);
			}).Get());

	//delete wpath;

	if (!SUCCEEDED(hr))
	{
		MessageBox(NULL, TEXT("fail"), TEXT("fail"), MB_OK);
		return false;
	}
	return true;
}

HRESULT BrowserWindow::CallbackOnCompleteWebView2Enviornment(ICoreWebView2Environment *env)
{
	/*
	size_t s = 0;
	int len = m_url.size();
	wchar_t *wurl = new wchar_t[len + 1];
	mbstowcs_s(&s, wurl, len, m_url.c_str(), _TRUNCATE);

	BYTE postDataBytes[] = { '1' };
	wil::com_ptr<IStream> postDataStream = SHCreateMemStream(postDataBytes, sizeof(postDataBytes));

	env->QueryInterface(IID_PPV_ARGS(&webviewEnviornment2));
	webviewEnviornment2->CreateWebResourceRequest(
		wurl,
		L"POST",
		postDataStream.get(),
		L"Content-Type: application/x-www-urlencoded",
		&webResourceRequest
	);
	*/

	env->CreateCoreWebView2Controller(m_hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
		[this](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
			return CallbackOnCompleteWebView2Handler(controller);
		}).Get());

	return S_OK;

}

HRESULT BrowserWindow::CallbackOnCompleteWebView2Handler(ICoreWebView2Controller *controller)
{
	if (controller != nullptr)
	{
		m_controlsController = controller;
		m_controlsController->get_CoreWebView2(&m_controlsWebView);
	}

	ICoreWebView2Settings* Settings;
	m_controlsWebView->get_Settings(&Settings);
	Settings->put_IsScriptEnabled(TRUE);
	Settings->put_AreDefaultScriptDialogsEnabled(TRUE);
	Settings->put_IsWebMessageEnabled(TRUE);

	wil::com_ptr<ICoreWebView2Settings2> settings2;
	m_controlsWebView->QueryInterface(IID_PPV_ARGS(&settings2));

	RECT bounds;
	GetClientRect(m_hWnd, &bounds);
	m_controlsController->put_Bounds(bounds);

	Navigate();

	return S_OK;
}

void BrowserWindow::SetURL(std::string url)
{
	m_url = url;
}

void BrowserWindow::Navigate()
{
	size_t s = 0;
	int len = m_url.size() + 1;
	wchar_t *wurl = new wchar_t[len];
	mbstowcs_s(&s, wurl, len, m_url.c_str(), _TRUNCATE);

	m_controlsWebView->Navigate(wurl);

	delete wurl;

	//webview2->NavigateWithWebResourceRequest(webResourceRequest.get());
}

void BrowserWindow::SetPosiSize(int x, int y, int w, int h)
{
	m_posX = x;
	m_posY = y;
	m_width = w;
	m_height = h;
}

void BrowserWindow::ProcOnPaint()
{

}

void BrowserWindow::ProcOnDestroy()
{

}





