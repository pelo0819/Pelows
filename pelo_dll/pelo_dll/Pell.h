#pragma once

#define MAX_KEY_COUNT 64
#define TITLE_SIZE 64
#define KEY_LOG_MEMORY "KEY_LOG_MEMORY"
#include "pch.h"

struct keyLogInfo
{
	char title[TITLE_SIZE];
	DWORD pid;
	WPARAM wp[MAX_KEY_COUNT];
	int keyCnt = 0;
	int time[5];
};

static HINSTANCE hdll;
void initialize();

LRESULT CALLBACK hookProc(int nCode, WPARAM wParam, LPARAM lParam);
LPCTSTR getSharedMemoryName(int id);
HANDLE setKeyLogSharedMemory(int windowCnt);
keyLogInfo* getKeyLogSharedMemory(int windowCnt);
HANDLE setSharedMemory(int size, LPCTSTR name);
template<typename T> T* getSharedMemory(int size, LPCTSTR name);
void initKeyLogInfo(keyLogInfo* info);
void printKeyLog(int windowId);
void writeKeyLog(int windwowId);
void getDllPath(std::string &path_str);
void refreshKeyLogSharedMemory(keyLogInfo* info);
void refreshKeyLogSharedMemory(int windowId);
void myMessageBox(std::string str);
