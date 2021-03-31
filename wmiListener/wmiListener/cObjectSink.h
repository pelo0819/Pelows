#pragma once
#include <wbemidl.h>
#include <string>
#include "MyParams.h"

/// <summary>
/// システム監視を行うComponent Object Model オブジェクト
/// </summary>
class CObjectSink :public IWbemObjectSink
{
public:
    CObjectSink();
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
    VOID MyMessageBox(std::string str);
private:
    LONG m_cRef;
};
