#include "CObjectSink.h"
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
    // COM�C���^�[�t�F�C�X����v���Z�X�����擾
    pObject->Get(L"Caption", 0, &var, 0, 0);

    SendMessage(Params::g_hwndListBox, LB_ADDSTRING, 0, (LPARAM)var.bstrVal);
    // �J��
    SysFreeString(bstr); // BSTR
    pObject->Release(); // COM�I�u�W�F�N�g
    VariantClear(&var); // VARIANT

    return WBEM_S_NO_ERROR;
}

STDMETHODIMP CObjectSink::SetStatus(long IFlags, HRESULT hResult, BSTR strParam, IWbemClassObject* pObjParam)
{
    return WBEM_S_NO_ERROR;
}



