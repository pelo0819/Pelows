#include "Params.h"

HWND Params::g_hwndListBox;
IWbemServices* Params::pNamespace = NULL;
IWbemObjectSink* Params::pStubSink = NULL;
