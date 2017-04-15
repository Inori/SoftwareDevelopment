#include <Windows.h>
#include "Plugin.h"


BOOL WINAPI DllEntryPoint(HINSTANCE hi, DWORD reason, LPVOID reserved) {
	//if (reason == DLL_PROCESS_ATTACH)
	//	hinst = hi;                          // Mark plugin instance
	return 1;                            // Report success
};


// Report plugin name and return version of plugin interface.
extc int _export cdecl ODBG_Plugindata(char shortname[32]) {
	strcpy(shortname, "OllyAntiTmd");    // Name of command line plugin
	return PLUGIN_VERSION;
};

// OllyDbg calls this obligatory function once during startup. I place all
// one-time initializations here. Parameter features is reserved for future
// extentions, do not use it.
extc int _export cdecl ODBG_Plugininit(int ollydbgversion, HWND hw, ulong *features)
{
	int a = 0;
	return a;
};

extc void _export cdecl ODBG_Pluginmainloop(DEBUG_EVENT* event)
{
	DWORD dwEventCode = 0;

	if (!event)
	{
		return;
	}

	dwEventCode = event->dwDebugEventCode;

	switch (dwEventCode)
	{
	case CREATE_PROCESS_DEBUG_EVENT:
		break;
	case LOAD_DLL_DEBUG_EVENT:
		MessageBoxA(NULL, event->u.LoadDll.lpImageName, "OllyAntiTmd", MB_OK);
		break;
	default:
		break;
	}
};

