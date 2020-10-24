#pragma once
#include "pch.h"

static HINSTANCE hdll;
void initialize();

extern "C" __declspec(dllexport) void StartHook();
extern "C" __declspec(dllexport) LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllexport) bool EndHook();

