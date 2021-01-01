#include "PeloUtils.h"

/// <summary>
/// Constructor
/// </summary>
PeloUtils::PeloUtils(){}

/// <summary>
/// 指定したプロセスIDからドメイン名、ユーザー名を設定する
/// </summary>
/// <param name="pid">プロセスid</param>
/// <returns>成功か失敗か</returns>
bool PeloUtils::SetProcessInfo(unsigned long pid)
{
	this->pid = pid;
	DWORD dwLength;
	HANDLE hToken;
	PTOKEN_USER pTokenUser;
	char szAccountName[256];
	DWORD dwAccountNameSize;
	char szDomainName[256];
	DWORD dwDomainNameSize;
	SID_NAME_USE snu;

	// プロセスのハンドルを取得
	HANDLE hProc = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE,
		pid
	);

	if (hProc == NULL) 
	{
		errLog = "[!!!] Failed OpenProcess()";
		return false;
	}

	// プロセスのトークンを取得
	if (!OpenProcessToken(hProc, TOKEN_QUERY, &hToken))
	{
		errLog = "[!!!] Failed OpenProcessToken()";
		return false;
	}

	// トークンの情報を取得
	GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLength);
	pTokenUser = (PTOKEN_USER)LocalAlloc(LPTR, dwLength);
	if (pTokenUser == NULL)
	{
		CloseHandle(hToken);
		errLog = "[!!!] Failed GetTokenInformation()";
		return false;
	}

	GetTokenInformation(hToken, TokenUser, pTokenUser, dwLength, &dwLength);
	dwAccountNameSize = sizeof(szAccountName) / sizeof(szAccountName[0]);
	dwDomainNameSize = sizeof(szDomainName) / sizeof(szDomainName[0]);
	LookupAccountSidA(NULL,
		pTokenUser->User.Sid,
		szAccountName,
		&dwAccountNameSize,
		szDomainName,
		&dwDomainNameSize,
		&snu);

	domain = szDomainName;
	userName = szAccountName;
	SetSidNameUse(snu);

	CloseHandle(hToken);
	LocalFree(pTokenUser);
	return true;
}

unsigned long PeloUtils::GetPid(){ return pid; }
std::string PeloUtils::GetPidStr()
{
	pidStr = std::to_string(pid);
	return pidStr; 
}
std::string PeloUtils::GetDomain(){ return domain; }
std::string PeloUtils::GetProcessUserName() { return userName; }
std::string PeloUtils::GetSid() { return sid; }
std::string PeloUtils::GetSidNameUse(){ return sidNameUse; }
std::string PeloUtils::GetAll()
{
	allInfo = "pid:" + GetPidStr() +
		"user:" + GetDomain() + "/" + GetProcessUserName();
	return allInfo;
}

void PeloUtils::SetSidNameUse(SID_NAME_USE snu)
{
	switch (snu) {
	case SidTypeUser:
		sidNameUse = "SidTypeUser";
		break;
	case SidTypeGroup:
		sidNameUse = "SidTypeGroup";
		break;
	case SidTypeDomain:
		sidNameUse = "SidTypeDomain";
		break;
	case SidTypeAlias:
		sidNameUse = "SidTypeAlias";
		break;
	case SidTypeWellKnownGroup:
		sidNameUse = "SidTypeWellKnownGroup";
		break;
	case SidTypeDeletedAccount:
		sidNameUse = "SidTypeDeletedAccount";
		break;
	case SidTypeInvalid:
		sidNameUse = "SidTypeInvalid";
		break;
	case SidTypeUnknown:
		sidNameUse = "SidTypeUnknown";
		break;
	case SidTypeComputer:
		sidNameUse = "SidTypeComputer";
		break;
	default:
		sidNameUse = "Unknown";
		break;
	}
}

