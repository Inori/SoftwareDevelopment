#include <Ntifs.h>
#include <ntddk.h>
#include "Undoc.h"
#include "HookFuncs.h"
#include "Tools.h"

//////////////////////////////////////////////////////////////////////////
#define	DEVICE_NAME			L"\\Device\\AntiTmd"
PDEVICE_OBJECT pDevObj;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID CreateProcessNotify(
	IN HANDLE  ParentId,
	IN HANDLE  ProcessId,
	IN BOOLEAN  Create
)
{
	NTSTATUS status;
	CHAR* szImageName = NULL;
	UNICODE_STRING usTargetName;
	PUNICODE_STRING pusImageName;
	PEPROCESS pEproc = NULL;

	//DbgBreakPoint();

	status = PsLookupProcessByProcessId(ProcessId, &pEproc);
	if (!NT_SUCCESS(status))
	{
		return;
	}

	RtlInitUnicodeString(&usTargetName, L"ErrorShowTMD.exe");

	pusImageName = GetProcNameByEproc(pEproc);
	if (!pusImageName)
	{
		return;
	}

	if (UsStr(pusImageName, &usTargetName) != 0)
	{
		DEBUG_PRINT("TargetProcess launched: %wZ", pusImageName);
		g_hTargetId = ProcessId;
	}

	if (pusImageName)
	{
		ExFreePool(pusImageName);
	}
}


VOID DriverUnload(PDRIVER_OBJECT pDriverObj)
{
	UnInstallHooks();
	PsSetCreateProcessNotifyRoutine(CreateProcessNotify, TRUE);
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegistryString)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING ustrDevName;
	void* pFunc = NULL;

	pDriverObj->DriverUnload = DriverUnload;

	PsSetCreateProcessNotifyRoutine(CreateProcessNotify, FALSE);


	InitUndocFunctions();

	InstallHooks();

	return STATUS_SUCCESS;
}