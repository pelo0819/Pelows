#pragma once
#include <wbemidl.h>
#include <string>
#include <atlstr.h>

class ProcessWatcher
{
public:
	ProcessWatcher(BSTR name);
	bool SetProcessInfo(unsigned long pId);
	bool SetDomainAndName(unsigned long pId);
	bool SetPrivilege(unsigned long pId);
	unsigned long GetPid();
	std::string GetName();
	std::string GetPidStr();
	std::string GetDomain();
	std::string GetProcessUserName();
	std::string GetSid();
	std::string GetSidNameUse();
	std::string GetPrivilege();
	std::string GetAll();
private:
	unsigned long pid = 0;
	std::string name = "";
	std::string pidStr = "";
	std::string domain = "";
	std::string userName = "";
	std::string sid = "";
	std::string sidNameUse = "";
	std::string privilege = "";
	std::string allInfo = "";
	std::string errLog = "";

	void SetSidNameUse(SID_NAME_USE snu);
};