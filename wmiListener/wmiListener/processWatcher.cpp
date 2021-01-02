#include "pch.h"
#include "processWatcher.h"

/// <summary>
/// Constructor
/// </summary>
ProcessWatcher::ProcessWatcher(BSTR n) 
{
	name = CStringA(n).GetBuffer();
}

/// <summary>
/// 指定したプロセスIDからドメイン名、ユーザー名を設定する
/// </summary>
/// <param name="pid">プロセスid</param>
/// <returns>成功か失敗か</returns>
bool ProcessWatcher::SetProcessInfo(unsigned long pid)
{
	this->pid = pid;
	SetDomainAndName(pid);
	SetPrivilege(pid);
	return true;
}

bool ProcessWatcher::SetDomainAndName(unsigned long pid)
{
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
		errLog = "[!!!] failed OpenProcess()";
		std::cout << errLog << std::endl;
		return false;
	}

	// プロセスのトークンを取得
	if (!OpenProcessToken(hProc, TOKEN_QUERY, &hToken))
	{
		errLog = "[!!!] failed OpenProcessToken()";
		std::cout << errLog << std::endl;
		CloseHandle(hToken);
		CloseHandle(hProc);
		return false;
	}

	// トークンの情報を取得
	GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLength);
	pTokenUser = (PTOKEN_USER)LocalAlloc(LPTR, dwLength);
	if (pTokenUser == NULL)
	{
		CloseHandle(hToken);
		CloseHandle(hProc);
		errLog = "[!!!] failed GetTokenInformation(), TokenUser";
		std::cout << errLog << std::endl;
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
	CloseHandle(hProc);
	LocalFree(pTokenUser);
	return true;
}

bool ProcessWatcher::SetPrivilege(unsigned long pid)
{
	HANDLE hToken;
	PTOKEN_PRIVILEGES pTokenPrivileges;
	char szPrivilegeName[256];
	char szDisplayName[256];
	DWORD dwLength;
	DWORD dwLanguageId;
	DWORD i;

	// プロセスのハンドルを取得
	HANDLE hProc = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE,
		pid
	);

	if (hProc == NULL)
	{
		errLog = "[!!!] failed OpenProcess()";
		std::cout << errLog << std::endl;
		return false;
	}

	// プロセスのトークンを取得
	if (!OpenProcessToken(hProc, TOKEN_QUERY, &hToken))
	{
		errLog = "[!!!] failed OpenProcessToken()";
		std::cout << errLog << std::endl;
		CloseHandle(hToken);
		CloseHandle(hProc);
		return false;
	}

	GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &dwLength);

	pTokenPrivileges = (PTOKEN_PRIVILEGES)LocalAlloc(LPTR, dwLength);
	if (pTokenPrivileges == NULL) {
		errLog = "[!!!] failed LocalAlloc()";
		std::cout << errLog << std::endl;
		CloseHandle(hToken);
		LocalFree(pTokenPrivileges);
		return false;
	}

	GetTokenInformation(hToken, TokenPrivileges, pTokenPrivileges, dwLength, &dwLength);

	for (DWORD i = 0; i < pTokenPrivileges->PrivilegeCount; i++) {

		dwLength = sizeof(szPrivilegeName) / sizeof(szPrivilegeName[0]);
		LookupPrivilegeNameA(NULL,
			&pTokenPrivileges->Privileges[i].Luid,
			szPrivilegeName,
			&dwLength);

		dwLength = sizeof(szDisplayName) / sizeof(szPrivilegeName[0]);
		LookupPrivilegeDisplayNameA(NULL,
			szPrivilegeName,
			szDisplayName,
			&dwLength,
			&dwLanguageId);
		privilege += szPrivilegeName;
		privilege += "| ";
	}

	CloseHandle(hToken);
	CloseHandle(hProc);
	LocalFree(pTokenPrivileges);
	return true;
}


unsigned long ProcessWatcher::GetPid() { return pid; }
std::string ProcessWatcher::GetName() { return name; }
std::string ProcessWatcher::GetPidStr()
{
	pidStr = std::to_string(pid);
	return pidStr;
}
std::string ProcessWatcher::GetDomain() { return domain; }
std::string ProcessWatcher::GetProcessUserName() { return userName; }
std::string ProcessWatcher::GetSid() { return sid; }
std::string ProcessWatcher::GetSidNameUse() { return sidNameUse; }
std::string ProcessWatcher::GetPrivilege() { return privilege; }
std::string ProcessWatcher::GetAll()
{
	allInfo = "pid:" + GetPidStr() +
		" name:" + GetName() +
		" user:" + GetDomain() + "/" + GetProcessUserName() +
		" privilege: " + GetPrivilege();
	return allInfo;
}

void ProcessWatcher::SetSidNameUse(SID_NAME_USE snu)
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

