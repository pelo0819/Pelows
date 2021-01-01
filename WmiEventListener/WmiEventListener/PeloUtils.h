#pragma once
#include <string>
//#include <sddl.h>
#include "framework.h"

class PeloUtils
{
public:
	PeloUtils();
	bool SetProcessInfo(unsigned long pId);
	bool SetDomainAndName(unsigned long pId);
	bool SetPrivilege(unsigned long pId);
	unsigned long GetPid();
	std::string GetPidStr();
	std::string GetDomain();
	std::string GetProcessUserName();
	std::string GetSid();
	std::string GetSidNameUse();
	std::string GetPrivilege();
	std::string GetAll();
	VOID MyMessageBox(std::string str);
private:
	unsigned long pid = 0;
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