#include "CObjectSink.h"
//#include "Params.h"

CObjectSink::CObjectSink()
{
    m_cRef = 1;
}

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
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IWbemObjectSink))
    {
        *ppvObject = static_cast<IWbemObjectSink*>(this);
    }
    else { return E_NOINTERFACE; }

    AddRef();
    /*MessageBox(NULL, TEXT("[OK] が押されました。"),
        TEXT("スタブが作成された"), MB_ICONINFORMATION);*/
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
STDMETHODIMP CObjectSink::Indicate(LONG IObjectCount, IWbemClassObject** ppObjArray)
{
    HRESULT hr;
    VARIANT var; // 汎用型
    BSTR bstr; //WCHAR
    IWbemClassObject* pObject;

    // プロパティ"Caption"を指定することでプロセス名を取得するためには、
    // 一度、"TargetInstance"を指定したりして準備する必要がある(作法?)
    // https://stackoverflow.com/questions/31753518/get-process-handle-of-created-processes-windows
    // BSTRの初期化
    bstr = SysAllocString(L"TargetInstance");
    //  ppObjArray[0]をvarにコピー
    ppObjArray[0]->Get(bstr, 0, &var, 0, 0);
    // COMインターフェイスを取得
    // IID_PPV_ARGS()でIIDとCOMオブジェクトを引数に指定してくれる
    var.pdispVal->QueryInterface(IID_PPV_ARGS(&pObject));
    VariantClear(&var);
    // COMインターフェイスからプロセス名を取得
    pObject->Get(L"Caption", 0, &var, 0, 0);

    SendMessage(Params::g_hwndListBox, LB_ADDSTRING, 0, (LPARAM)var.bstrVal);
    // 開放
    SysFreeString(bstr); // BSTR
    pObject->Release(); // COMオブジェクト
    VariantClear(&var); // VARIANT

    return WBEM_S_NO_ERROR;
}

STDMETHODIMP CObjectSink::SetStatus(long IFlags, HRESULT hResult, BSTR strParam, IWbemClassObject* pObjParam)
{
    return WBEM_S_NO_ERROR;
}




