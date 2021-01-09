#pragma once
#include <wbemidl.h>
#include <string>
#include <atlstr.h>
#include <fstream>
#include "myParams.h"

class ProcessWatcher
{
public:
	ProcessWatcher();
	bool setDate();
	bool setProcessInfo(unsigned long pId);
	bool setDomainAndName(unsigned long pId);
	bool setPrivilege(unsigned long pId);
	void setName(BSTR name);
	bool setCommandLine(BSTR command);
	void setErrorLog(std::string e);
	void setLastError(unsigned long d);
	unsigned long getPid();
	std::string getName();
	std::string getCommandLine();
	std::string getDate();
	std::string getPidStr();
	std::string getDomain();
	std::string getProcessUserName();
	std::string getSid();
	std::string getSidNameUse();
	std::string getPrivilege();
	std::string getAll();
	std::string getErr();
	void writeKeyLog();
	void getDllPath(std::string& path_str);
private:
	unsigned long pid = 0;
	std::string date = "";
	std::string name = "";
	std::string commandLine = "";
	std::string pidStr = "";
	std::string domain = "";
	std::string userName = "";
	std::string sid = "";
	std::string sidNameUse = "";
	std::string privilege = "";
	std::string allInfo = "";
	std::string errLog = "";
	void setSidNameUse(SID_NAME_USE snu);
};