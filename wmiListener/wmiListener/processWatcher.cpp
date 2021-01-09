#include "pch.h"
#include "processWatcher.h"

/// <summary>
/// Constructor
/// </summary>
ProcessWatcher::ProcessWatcher()
{
}

void ProcessWatcher::setName(BSTR n)
{
	name = CStringA(n).GetBuffer();
	setDate();
}

bool ProcessWatcher::setCommandLine(BSTR c)
{
	commandLine = CStringA(c).GetBuffer();
	return true;
}

bool ProcessWatcher::setDate()
{
	time_t t = time(NULL);
	struct tm tM;
	errno_t err_t = localtime_s(&tM, &t);
	int year = tM.tm_year + 1900;
	int mon = tM.tm_mon + 1;
	int day = tM.tm_mday;
	int hour = tM.tm_hour;
	int min = tM.tm_min;
	std::string yearStr = std::to_string(year);
	std::string monStr = std::to_string(mon);
	if (monStr.length() <= 1) { monStr = "0" + monStr; }
	std::string dayStr = std::to_string(day);
	if (dayStr.length() <= 1) { dayStr = "0" + dayStr; }
	std::string hourStr = std::to_string(hour);
	if (hourStr.length() <= 1) { hourStr = "0" + hourStr; }
	std::string minStr = std::to_string(min);
	if (minStr.length() <= 1) { minStr = "0" + minStr; }

	date = yearStr + "-" + monStr + "-" + dayStr + "T" + hourStr
		+ ":" + minStr + "+09:00";
	return true;
}

std::string ProcessWatcher::getDate() { return date; }


/// <summary>
/// 指定したプロセスIDからドメイン名、ユーザー名を設定する
/// </summary>
/// <param name="pid">プロセスid</param>
/// <returns>成功か失敗か</returns>
bool ProcessWatcher::setProcessInfo(unsigned long pid)
{
	this->pid = pid;
	if (pid == 0) 
	{
		setErrorLog("[!!!] failed get process id");
		return false;
	}
	setDomainAndName(pid);
	setPrivilege(pid);
	return true;
}

bool ProcessWatcher::setDomainAndName(unsigned long pid)
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
		setErrorLog("[!!!] failed OpenProcess in SetDomainAndName");
		setLastError(GetLastError());
		return false;
	}

	// プロセスのトークンを取得
	if (!OpenProcessToken(hProc, TOKEN_QUERY, &hToken))
	{
		setErrorLog("[!!!] failed OpenProcessToken in SetDomainAndName");
		CloseHandle(hToken);
		CloseHandle(hProc);
		return false;
	}

	// トークンの情報を取得
	GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLength);
	pTokenUser = (PTOKEN_USER)LocalAlloc(LPTR, dwLength);
	if (pTokenUser == NULL)
	{
		setErrorLog("[!!!] failed GetTokenInformation in SetDomainAndName");
		CloseHandle(hToken);
		CloseHandle(hProc);
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
	setSidNameUse(snu);

	CloseHandle(hToken);
	CloseHandle(hProc);
	LocalFree(pTokenUser);
	return true;
}

bool ProcessWatcher::setPrivilege(unsigned long pid)
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
		setErrorLog("[!!!] failed OpenProcess in SetPrivilege");
		setLastError(GetLastError());
		return false;
	}

	// プロセスのトークンを取得
	if (!OpenProcessToken(hProc, TOKEN_QUERY, &hToken))
	{
		setErrorLog("[!!!] failed OpenProcessToken in SetPrivilege");
		CloseHandle(hToken);
		CloseHandle(hProc);
		return false;
	}

	GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &dwLength);

	pTokenPrivileges = (PTOKEN_PRIVILEGES)LocalAlloc(LPTR, dwLength);
	if (pTokenPrivileges == NULL) {
		setErrorLog("[!!!] failed LocalAlloc in SetPrivilege");
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


unsigned long ProcessWatcher::getPid() { return pid; }
std::string ProcessWatcher::getName() { return name; }
std::string ProcessWatcher::getCommandLine() { return commandLine; }
std::string ProcessWatcher::getPidStr()
{
	pidStr = std::to_string(pid);
	return pidStr;
}
std::string ProcessWatcher::getDomain() { return domain; }
std::string ProcessWatcher::getProcessUserName() { return userName; }
std::string ProcessWatcher::getSid() { return sid; }
std::string ProcessWatcher::getSidNameUse() { return sidNameUse; }
std::string ProcessWatcher::getPrivilege() { return privilege; }
std::string ProcessWatcher::getErr() { return errLog; }
std::string ProcessWatcher::getAll()
{
	allInfo = "pid:" + getPidStr() +
		" name:" + getName() +
		" user:" + getDomain() + "/" + getProcessUserName() +
		" privilege: " + getPrivilege();
	return allInfo;
}

void ProcessWatcher::setSidNameUse(SID_NAME_USE snu)
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

void ProcessWatcher::setErrorLog(std::string e)
{
	errLog += e + " / ";
}

/// <summary>
/// 
/// </summary>
/// <param name="d">DWORD : unsigned long</param>
void ProcessWatcher::setLastError(unsigned long d)
{
	// error index info
	// http://gallery-code.blogspot.com/2010/05/getlasterror.html
	setErrorLog("last error index:" + std::to_string(d));
	/*switch (d)
	{
	case 0:
		setErrorLog("no error. ");
		break;
	case 1:
		setErrorLog("not found function name. ");
		break;
	case 2:
		setErrorLog("cannot find file. ");
		break;
	case 3:
		setErrorLog("cannot find path. ");
		break;
	case 4:
		setErrorLog("cannot open file. ");
		break;
	case 5:
		setErrorLog("access denied. ");
		break;
	case 6:
		setErrorLog("invalid handle. ");
		break;
	case 7:
		setErrorLog("miss function");
		break;
	case 1:
		setErrorLog("miss function");
		break;
	case 1:
		setErrorLog("miss function");
		break;
	case 1:
		setErrorLog("miss function");
		break;
	case 1:
		setErrorLog("miss function");
		break;
	case 1:
		setErrorLog("miss function");
		break;
	case 1:
		setErrorLog("miss function");
	case 1:
		setErrorLog("miss function");
		break;
	default:
		break;
	}*/
}


void ProcessWatcher::writeKeyLog()
{
	std::ofstream outputfile;
	std::string path_str = "";
	getDllPath(path_str);
	path_str += "/.peloWatch";

	outputfile.open(path_str, std::ios::app);
	outputfile << "pid:" << getPidStr() << std::endl;
	outputfile << "date:" << getDate() << std::endl;
	outputfile << "name:" << getName() << std::endl;
	outputfile << "command:" << getCommandLine() << std::endl;
	outputfile << "username:" << getDomain() << "/" << getProcessUserName() << std::endl;
	outputfile << "privilege:" << getPrivilege() << std::endl;
	outputfile << "err:" << getErr() << std::endl;
	outputfile << "  " << std::endl;
	outputfile.close();
}

void ProcessWatcher::getDllPath(std::string& path_str)
{
	char path[MAX_PATH];
	int size = sizeof(path) / sizeof(path[0]);
	GetModuleFileNameA(MyParams::hInst, path, size);
	for (int i = 0; i < size; i++)
	{
		path_str += path[i];
	}
	int pos = path_str.rfind("dll");
	path_str = path_str.substr(0, pos);
	pos = path_str.rfind("\\");
	path_str = path_str.substr(0, pos);
}


