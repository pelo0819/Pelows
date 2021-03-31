#pragma once
#include <wbemidl.h>
#include <string>
#include "MyParams.h"

/// <summary>
/// �V�X�e���Ď����s��Component Object Model �I�u�W�F�N�g
/// </summary>
class CObjectSink :public IWbemObjectSink
{
public:
    CObjectSink();
    // STDMETHODIMP: HRESULT STDMETHODCALLTYPE
    // HRESULT : long , ���������s�������łȂ����s�����ꍇ�̗��R������
    // https://www.usefullcode.net/2007/03/hresult.html
    // STDMETHODCALLTYPE : __stdcall �Ăяo���K��
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
