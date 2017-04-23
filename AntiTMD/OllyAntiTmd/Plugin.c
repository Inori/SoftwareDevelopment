#include <Windows.h>
#include "Plugin.h"
#include "Tools.h"


HINSTANCE g_hDllInst = NULL;
BOOL g_bFirstLoadDll = TRUE;
BOOL g_bFirstException = TRUE;

BOOL WINAPI DllEntryPoint(HINSTANCE hi, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		g_hDllInst = hi;
	}
	return 1;                            
};


extc int _export cdecl ODBG_Plugindata(char shortname[32]) 
{
	strcpy(shortname, "OllyAntiTmd");    // Name of command line plugin
	return PLUGIN_VERSION;
};

// OllyDbg calls this obligatory function once during startup. I place all
// one-time initializations here. Parameter features is reserved for future
// extentions, do not use it.
extc int _export cdecl ODBG_Plugininit(int ollydbgversion, HWND hw, ulong *features)
{
	g_bFirstLoadDll = TRUE;
	g_bFirstException = TRUE;
	return 0;
};

extc void _export cdecl ODBG_Pluginreset(void)
{
	g_bFirstLoadDll = TRUE;
	g_bFirstException = TRUE;
}

VOID OnLoadDllEvent(DEBUG_EVENT* event)
{
	DWORD dwPid = 0;
	static BOOL bSecondLoadDll = FALSE;
	if (!event)
	{
		return;
	}
	dwPid = event->dwProcessId;

	if (g_bFirstLoadDll)
	{
		SetRemotePebBeingDebuged(dwPid, 0);
		g_bFirstLoadDll = FALSE;
		bSecondLoadDll = TRUE;
	}
	else if (bSecondLoadDll)
	{
		SetRemotePebBeingDebuged(dwPid, 1);
		bSecondLoadDll = FALSE;
	}
}

void OnExceptionEvent(DEBUG_EVENT* event)
{
	DWORD dwPid = 0;
	if (!event)
	{
		return;
	}
	dwPid = event->dwProcessId;

	if (g_bFirstException && event->u.Exception.ExceptionRecord.ExceptionCode == STATUS_BREAKPOINT)
	{
		SetRemotePebBeingDebuged(dwPid, 0);
		g_bFirstException = FALSE;
	}
}

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
	case EXCEPTION_DEBUG_EVENT:
		OnExceptionEvent(event);
		break;
	case CREATE_PROCESS_DEBUG_EVENT:
		break;
	case LOAD_DLL_DEBUG_EVENT:
		OnLoadDllEvent(event);
		break;
	default:
		break;
	}
};

