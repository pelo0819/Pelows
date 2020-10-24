#pragma once
#include "pch.h"

static HINSTANCE hdll;
void initialize();
void StartHook();
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
bool EndHook();

