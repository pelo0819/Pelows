#include "pch.h"
#include <wbemidl.h>
#include "cObjectSink.h"


#pragma comment (lib, "wbemuuid.lib")

#pragma region 作法  
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#pragma endregion

static IWbemServices* pNamespace = NULL;
static IWbemObjectSink* pStubSink = NULL;

extern "C" __declspec(dllexport) bool init()
{
    std::cout << "[*] start to inialize process watcher." << std::endl;

    HRESULT hr;
    BSTR bstrNamespace;
    BSTR bstrQuery;
    BSTR bstrLanguage;
    IWbemLocator* pLocator;
    IUnsecuredApartment* pUnsecApp;
    IUnknown* pStubUnk;
    CObjectSink* pObjectSink;

    // アパートメントをMTAに設定
    // https://www.kekyo.net/2013/07/22/382
    hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr)) { std::cout << "[!!!] failed CoInitializeEx()" << std::endl; }

    // WMIはDCOM(COMの改良版)を使用している。DCOMはRPCを使用していて、
    // RPCの通信にはセキュリティ情報を指定する必要がある
    hr = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL);
    if (FAILED(hr)) { std::cout << "[!!!] failed CoInitializeSecurity()" << std::endl; }

    // COMのインスタンスを作成
    hr = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pLocator));
    if (FAILED(hr)) { std::cout << "[!!!] failed CoCreateInstance()" << std::endl; }

    // 名前空間に接続してIWbemServices(pNamespace)を取得
    // IWbemServicesからWMIにアクセスできる
    bstrNamespace = SysAllocString(L"root\\cimv2");
    hr = pLocator->ConnectServer(
        bstrNamespace,
        NULL,
        NULL,
        NULL,
        0,
        NULL,
        NULL,
        &pNamespace);

    if (FAILED(hr))
    {
        std::cout << "[!!!] failed ConnectServer()" << std::endl;
        SysFreeString(bstrNamespace);
        pLocator->Release();
        return false;
    }

    // pNameSpaceに個別の設定を行う(MSDNのサンプル)
    hr = CoSetProxyBlanket(
        pNamespace,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE);

    // 下記で述べるスタブの作成
    // スタブを作成するには、CreateObjectStub()を実行する必要がある
    // CreateObjectStub()へのアクセスは、IUnsecuredApartment(pUnsecApp)から可能
    // 第1引数をCLSID_UnsecuredApartmentとしてCoCreateInstance()を実行すると
    // IUnsecuredApartmentを取得できる
    hr = CoCreateInstance(
        CLSID_UnsecuredApartment,
        NULL,
        CLSCTX_LOCAL_SERVER,
        IID_PPV_ARGS(&pUnsecApp));

    pObjectSink = new CObjectSink;
    pUnsecApp->CreateObjectStub(pObjectSink, &pStubUnk);
    pStubUnk->QueryInterface(IID_IWbemObjectSink, (void**)&pStubSink);

    // 取得したいイベントのクエリを定義
    bstrQuery = SysAllocString(L"SELECT * FROM __InstanceCreationEvent WITHIN 1 WHERE TargetInstance ISA 'Win32_Process'");
    //bstrQuery = SysAllocString(L"SELECT * FROM __InstanceOperationEvent WITHIN 1 WHERE TargetInstance ISA 'CIM_DataFile' AND TargetInstance.Drive='C:' AND TargetInstance.Path='\\\\Users\\\\tobita\\\\Desktop\\\\' AND TargetInstance.Extension='txt'");
    //bstrQuery = SysAllocString(L"SELECT * FROM __InstanceOperationEvent  WITHIN 1 WHERE TargetInstance ISA 'Win32_PingStatus' AND TargetInstance.Address = '192.168.3.5'");
    bstrLanguage = SysAllocString(L"WQL");

    // イベントが発生した場合、IWbemObjectSink(pStubSink)に通知がいくように設定
    // 第5引数は、CObjectSinkオブジェクトを入れるわけではなく、スタブを入れないとだめらしい
    // イベントの通知はプロバイダが行うので、クライアントで定義したCObjectSinkクラスのこと知らないから
    hr = pNamespace->ExecNotificationQueryAsync(
        bstrLanguage,
        bstrQuery,
        WBEM_FLAG_SEND_STATUS,
        NULL,
        pStubSink);

    if (FAILED(hr))
    {
        std::cout << "[!!!] failed ExecNotificationQueryAsync()" << std::endl;
    }

    SysFreeString(bstrLanguage);
    SysFreeString(bstrQuery);
    SysFreeString(bstrNamespace);
    pObjectSink->Release();
    pStubUnk->Release();
    pUnsecApp->Release();
    pLocator->Release();
    
    std::cout << "[*] success  to start process watcher" << std::endl;
    
    return true;
}

extern "C" __declspec(dllexport) void end()
{
    if (pNamespace != NULL) {
        if (pStubSink != NULL) {
            pNamespace->CancelAsyncCall(pStubSink);
            pStubSink->Release();
        }
        pNamespace->Release();
    }
    CoUninitialize();
}



