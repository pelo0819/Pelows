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
	static ATOM RegisterClass(_In_ HINSTANCE hInstance);
	static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static BOOL LaunchWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow, HWND hWnd);

protected:
	HINSTANCE m_hInst = nullptr;  // Current app instance
	HWND m_hWnd = nullptr;
	HWND m_parent = nullptr;

	static WCHAR s_windowClass[MAX_LOADSTRING];  // The window class name
	static WCHAR s_title[MAX_LOADSTRING];  // The title bar text

	int m_minWindowWidth = 0;
	int m_minWindowHeight = 0;

	Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controlsController;
	Microsoft::WRL::ComPtr<ICoreWebView2> m_controlsWebView;

	EventRegistrationToken m_controlsUIMessageBrokerToken = {};  // Token for the UI message handler in controls WebView
	EventRegistrationToken m_controlsZoomToken = {};
	EventRegistrationToken m_optionsUIMessageBrokerToken = {};  // Token for the UI message handler in options WebView
	EventRegistrationToken m_optionsZoomToken = {};
	EventRegistrationToken m_lostOptionsFocus = {};  // Token for the lost focus handler in options WebView
	Microsoft::WRL::ComPtr<ICoreWebView2WebMessageReceivedEventHandler> m_uiMessageBroker;

	BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND hWnd);
};