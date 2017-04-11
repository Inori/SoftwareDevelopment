#include <ntddk.h>

#include "SSDTHook.h"
#include "Tools.h"
#include "KernelInlineHook.h"

BOOLEAN HookSSDT(ULONG index, PVOID ProxyFunc, PVOID* OldFunc)
{
	PLONG ServiceTableBase = NULL;

	if (!KeServiceDescriptorTable || !ProxyFunc || !OldFunc) 
	{
		return FALSE;
	}

	if (index >= KeServiceDescriptorTable->NumberOfServices)
	{
		return FALSE;
	}

	ServiceTableBase = KeServiceDescriptorTable->ServiceTableBase;

	DisableWriteProtect();
	*OldFunc = ServiceTableBase[index];
	ServiceTableBase[index] = ProxyFunc;
	EnableWriteProtect();

	return TRUE;
}

BOOLEAN UnHookSSDT(ULONG index, PVOID OldFunc)
{
	PLONG ServiceTableBase = NULL;

	if (!KeServiceDescriptorTable || !OldFunc)
	{
		return FALSE;
	}

	if (index >= KeServiceDescriptorTable->NumberOfServices)
	{
		return FALSE;
	}

	DisableWriteProtect();
	ServiceTableBase = KeServiceDescriptorTable->ServiceTableBase;
	ServiceTableBase[index] = OldFunc;
	EnableWriteProtect();

	return TRUE;
}

