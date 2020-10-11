#include "resource.h"
#include "WindowProc.h"
#include "KeyHook.h"

int paintCnt = 0;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;
    LPCWSTR lpszStr = TEXT("hello");

    switch (message)
    {
    case WM_CREATE:
        lpszStr = TEXT("GoodBYE");
        StartHook(hWnd);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // ëIëÇ≥ÇÍÇΩÉÅÉjÉÖÅ[ÇÃâêÕ:
        switch (wmId)
        {
        case IDM_ABOUT:
            //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
        GetClientRect(hWnd, &rc);
        HDC hdc = BeginPaint(hWnd, &ps);
        wchar_t szText[3] = L"ss";
        int num = 5555;
        std::string g = std::to_string(GetCallbackCnt());
        std::string text = "jjjj1356" + g;
        MyDrawText(hdc, text, &rc, DT_CENTER);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        EndHook();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;

}
