#pragma once

//#include <wrl.h>
//#include <wil/com.h>
// include WebView2 header
//#include "WebView2.h"
#include "framework.h"
//#include "Resource.h"

class BrowserWindow
{
public:
	ATOM RegisterClass(_In_ HINSTANCE hInstance);
	bool LaunchWindow();
	void SetParentWindow(HWND hParent);
	HWND CreateBrowserWindow();
	void SetURL(std::string url);
	void Navigate();
	void SetPosiSize(int x, int y, int w, int h);
	void ProcOnPaint();
	void ProcOnDestroy();

protected:
	const char *windowClass = "BrowserWindow";
	HINSTANCE m_hInst = nullptr;  // Current app instance
	HWND m_hWnd = nullptr;
	HWND m_hWndParent = nullptr;

	int m_posX = 500;
	int m_posY = 300;
	int m_width = 500;
	int m_height = 300;

	std::string m_url;

	Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controlsController;
	Microsoft::WRL::ComPtr<ICoreWebView2> m_controlsWebView;
	wil::com_ptr<ICoreWebView2Environment2> webviewEnviornment2;
	wil::com_ptr<ICoreWebView2WebResourceRequest> webResourceRequest;
	wil::com_ptr<ICoreWebView2_2> webview2;

	bool SetupWebView2();
	HRESULT CallbackOnCompleteWebView2Enviornment(ICoreWebView2Environment *env);
	HRESULT CallbackOnCompleteWebView2Handler(ICoreWebView2Controller *controller);
};