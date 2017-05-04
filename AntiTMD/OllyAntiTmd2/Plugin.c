#include <Windows.h>
#include "Plugin.h"
#include "../PluginPublic/Tools.h"


#define ANTI_TMD_NAME L"OllyAntiTmd"
#define ANTI_TMD_VERSION L"1.0.0"
#define ANTI_TMD_WIND_TITLE L"OllyAsuka"

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


extc int _export cdecl ODBG2_Pluginquery(int ollydbgversion, ulong *features,
	wchar_t pluginname[SHORTNAME], wchar_t pluginversion[SHORTNAME])
{
	if (ollydbgversion < 201)
		return 0;
	wcscpy(pluginname, ANTI_TMD_NAME);       // Name of plugin
	wcscpy(pluginversion, ANTI_TMD_VERSION);       // Version of plugin
	return PLUGIN_VERSION;               // Expected API version
};

// OllyDbg calls this obligatory function once during startup. I place all
// one-time initializations here. Parameter features is reserved for future
// extentions, do not use it.
extc int _export cdecl ODBG2_Plugininit()
{
	g_bFirstLoadDll = TRUE;
	g_bFirstException = TRUE;

	SetWindowTextW(hwollymain, ANTI_TMD_WIND_TITLE);

	return 0;
};


extc void _export cdecl ODBG2_Pluginreset(void)
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

extc void _export cdecl ODBG2_Pluginmainloop(DEBUG_EVENT* event)
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


static int Mabout(t_table *pt, wchar_t *name, ulong index, int mode)
{
	if (mode == MENU_VERIFY)
		return MENU_NORMAL;                // Always available
	return MENU_ABSENT;
};


static t_menu mainmenu[] = {
	{ L"OllyAntiTmd",
	L"OllyAntiTmd",
	K_NONE, Mabout, NULL, 0 },
	{ NULL, NULL, K_NONE, NULL, NULL, 0 }
};


extc t_menu * __cdecl ODBG2_Pluginmenu(wchar_t *type) 
{
	if (wcscmp(type, PWM_MAIN) == 0)
		// Main menu.
		return mainmenu;
	return NULL;                         // No menu
}

