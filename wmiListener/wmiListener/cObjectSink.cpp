#include "pch.h"
#include "cObjectSink.h"
#include "processWatcher.h"
#include <string>
#include <stdio.h>
#include <time.h>

//#include "Params.h"

CObjectSink::CObjectSink()
{
    m_cRef = 1;
}

/// <summary>
/// ����COM�I�u�W�F�N�g���T�|�[�g���Ă���COM�C���^�[�t�F�C�X���擾
/// COM�I�u�W�F�N�g:
/// Component Object Model(COM)�́A�֗��ȋ@�\��񋟂���v���O����
/// ���̃v���O�������s���ɂ̓�������Ɏ��̂����������
/// �������ꂽ���̂̂��Ƃ�COM�I�u�W�F�N�g�Ƃ���
/// COM�C���^�[�t�F�C�X: COM�I�u�W�F�N�g���O�����痘�p���邽�߂̃A�N�Z�X�ӏ�
/// </summary>
/// <param name="riid">�C���^�[�t�F�C�X��GUID</param>
/// <param name="ppvObject">�I�u�W�F�N�g�̃|�C���^�̃|�C���^</param>
/// <returns></returns>
STDMETHODIMP CObjectSink::QueryInterface(REFIID riid, void** ppvObject)
{
    *ppvObject = NULL;

    // �w�肵��IID��COM�I�u�W�F�N�g������COM�C���^�[�t�F�C�XIID����v���邩�m�F
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IWbemObjectSink))
    {
        *ppvObject = static_cast<IWbemObjectSink*>(this);
    }
    else { return E_NOINTERFACE; }

    AddRef();
    /*MessageBox(NULL, TEXT("[OK] ��������܂����B"),
        TEXT("�X�^�u���쐬���ꂽ"), MB_ICONINFORMATION);*/
    return S_OK;
}

/// <summary>
/// �Q�ƃJ�E���^���X�V����
/// �����X���b�h�ŃI�u�W�F�N�g�𑀍삷��ꍇ�A������
/// �ϐ����g�p���邱�Ƃ�h�����߁AInterlockedIncrement()���g��
/// </summary>
/// <returns></returns>
STDMETHODIMP_(ULONG) CObjectSink::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

/// <summary>
/// COM�I�u�W�F�N�g�̊J���E�j�����s��
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
/// �C�x���g�����������ꍇ�ɌĂ΂��֐�
/// </summary>
/// <param name="IObjectCount"></param>
/// <param name="ppObjArray"></param>
/// <returns></returns>
STDMETHODIMP CObjectSink::Indicate(LONG IObjectCount, IWbemClassObject** ppObjArray)
{
    HRESULT hr;
    VARIANT var; // �ėp�^
    BSTR bstr; //WCHAR
    IWbemClassObject* pObject;


    // �v���p�e�B"Caption"���w�肷�邱�ƂŃv���Z�X�����擾���邽�߂ɂ́A
    // ��x�A"TargetInstance"���w�肵���肵�ď�������K�v������(��@?)
    // https://stackoverflow.com/questions/31753518/get-process-handle-of-created-processes-windows
    // BSTR�̏�����
    bstr = SysAllocString(L"TargetInstance");

    //  ppObjArray[0]��var�ɃR�s�[
    ppObjArray[0]->Get(bstr, 0, &var, 0, 0);

    // COM�C���^�[�t�F�C�X���擾
    // IID_PPV_ARGS()��IID��COM�I�u�W�F�N�g�������Ɏw�肵�Ă����
    var.pdispVal->QueryInterface(IID_PPV_ARGS(&pObject));
    VariantClear(&var);

    ProcessWatcher* watch;
    watch = new ProcessWatcher();

    // COM�C���^�[�t�F�C�X����v���Z�X�̏��X�����擾
    hr = pObject->Get(L"ExecutablePath", 0, &var, 0, 0);
    if (FAILED(hr))
    {
        watch->setErrorLog("[!!!] failed Get ExecutablePath");
    }
    watch->setName(var.bstrVal);

    hr = pObject->Get(L"CommandLine", 0, &var, 0, 0);
    if (FAILED(hr))
    {
        watch->setErrorLog("[!!!] failed Get CommandLine");
    }
    watch->setCommandLine(var.bstrVal);

    hr = pObject->Get(L"ProcessId", 0, &var, 0, 0);
    if (SUCCEEDED(hr))
    {
        watch->setProcessInfo(var.ulVal);
    }
    else
    {
        watch->setErrorLog("[!!!] failed Get ProcessId");
    }
    watch->writeKeyLog();
    // �J��
    SysFreeString(bstr); // BSTR
    pObject->Release(); // COM�I�u�W�F�N�g
    VariantClear(&var);
    delete watch;
    return WBEM_S_NO_ERROR;
}

STDMETHODIMP CObjectSink::SetStatus(long IFlags, HRESULT hResult, BSTR strParam, IWbemClassObject* pObjParam)
{
    return WBEM_S_NO_ERROR;
}

VOID CObjectSink::MyMessageBox(std::string str)
{
    int len = strlen(str.c_str());
    wchar_t t[128] = L"";
    if (len > 128) { len = 128; }
    for (int i = 0; i < len; i++)
    {
        t[i] = str[i];
    }
    MessageBoxW(NULL, t, TEXT("click"), MB_OK);
}
