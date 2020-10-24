#pragma once
#include "pch.h"



class KeyboardTracer
{
private:
	int *lp_binary[32] = {};
	std::string key_log;
public:
	KeyboardTracer();
	virtual ~KeyboardTracer();
	void setLpBinary(LPARAM lparam);
	int* getLpBinary();
	int getLpBinary(int n);
	void registKeyLog(WPARAM virkey);
	void print_log();
	std::string transVirtualKey(WPARAM virkey);
};

