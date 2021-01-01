#pragma once

#include "framework.h"
#include <wbemidl.h>

class Params 
{
public:
	static HWND g_hwndListBox;
	static IWbemServices* pNamespace;
	static IWbemObjectSink* pStubSink;
};

