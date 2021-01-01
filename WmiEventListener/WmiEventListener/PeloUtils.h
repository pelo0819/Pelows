#pragma once
#include <string>
#include <sddl.h>
#include "framework.h"

class PeloUtils
{
public:
	PeloUtils();
	bool SetProcessInfo(unsigned long pId);
	unsigned long GetPid();
	std::string GetPidStr();
	std::string GetDomain();
	std::string GetProcessUserName();
	std::string GetSid();
	std::string GetSidNameUse();
	std::string GetAll();
private:
	unsigned long pid = 0;
	std::string pidStr = "";
	std::string domain = "";
	std::string userName = "";
	std::string sid = "";
	std::string sidNameUse = "";
	std::string allInfo = "";
	std::string errLog = "";

	void SetSidNameUse(SID_NAME_USE snu);
};