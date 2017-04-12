#include <Ntifs.h>
#include <ntddk.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "KernelInlineHook.h"
#include "Tools.h"
#include "SSDTHook.h"
#include "UndocType.h"
#include "SSDTNumber.h"


//////////////////////////////////////////////////////////////////////////
#define	DEVICE_NAME			L"\\Device\\AntiTmd"
#define ALLOC_TAG 'ANTI'
#define MAX_PATH	260
PDEVICE_OBJECT pDevObj;


HANDLE g_hTargetId = 0;
//////////////////////////////////////////////////////////////////////////
#define DBG_STR_LEN 1024
void DebugPrint(const char* function, const char* format, ...)
{
	char szTemp[DBG_STR_LEN] = { 0 };
	char szDbgStr[DBG_STR_LEN] = { 0 };

	va_list   arg_list;

	sprintf(szDbgStr, "Asuka->[%s]\t", function);

	va_start(arg_list, format);

	vsprintf(szTemp, format, arg_list);

	va_end(arg_list);

	strcat(szDbgStr, szTemp);
	strcat(szDbgStr, "\n");
	DbgPrint(szDbgStr);
}

#define DEBUG_PRINT(format, ...) DebugPrint(__FUNCTION__, format, __VA_ARGS__);



//////////////////////////////////////////////////////////////////////////

PUNICODE_STRING GetProcNameByEproc(IN PEPROCESS pEproc)
/*++

Routine Description:

获取指定进程的完整进程名

Arguments:

pEproc - 指定进程的EPROCESS地址

Return Value:

成功则返回进程名，失败返回NULL

Comments:

该函数返回的进程名，由调用则负责释放（ExFreePool）。
--*/
{
	NTSTATUS NtStatus;
	HANDLE hProc = NULL;
	CHAR* pBuf = NULL;
	ULONG ulSize = 32;

	PAGED_CODE();

	// 
	// 1. pEproc --> handle 
	// 
	NtStatus = ObOpenObjectByPointer(pEproc,
		OBJ_KERNEL_HANDLE,
		NULL,
		0,
		NULL,
		KernelMode,
		&hProc
	);

	if (!NT_SUCCESS(NtStatus))
		return NULL;

	// 
	// 2. ZwQueryInformationProcess 
	// 
	while (TRUE)
	{
		pBuf = ExAllocatePoolWithTag(NonPagedPool, ulSize, ALLOC_TAG);
		if (!pBuf) {
			ZwClose(hProc);
			return NULL;
		}

		NtStatus = ZwQueryInformationProcess(hProc,
			ProcessImageFileName,
			pBuf,
			ulSize,
			&ulSize);
		if (NtStatus != STATUS_INFO_LENGTH_MISMATCH)
			break;

		ExFreePool(pBuf);
	}

	ZwClose(hProc);

	if (!NT_SUCCESS(NtStatus)) {
		ExFreePool(pBuf);
		return NULL;
	}

	// 
	// 3. over 
	// 
	return (PUNICODE_STRING)pBuf;
}



BOOLEAN IsTargetProcess()
{
	HANDLE hPid = 0;

	hPid = PsGetCurrentProcessId();

	return (hPid == g_hTargetId);
}

VOID* FindKiDispatchException()
{
	PLONG ServiceTableBase = NULL;
	PUCHAR pCallAddr = 0;
	ULONG nCallVal = 0;
	VOID* pNtRaiseException = NULL;
	VOID* pKiRaiseException = NULL;
	VOID* pKiDispatchException = NULL;

	//Xp SP3
	CHAR pKRETag[] = { 0x52, 0x53, 0x6a, 0x00, 0x51, 0x50 };
	ULONG nKRETagLen = sizeof(pKRETag);

	CHAR pKDETag[] = {
		0xff,0x75,0x18,
		0xff,0xb5,0x00,0xfd,0xff,0xff,
		0xff,0xb5,0x0c,0xfd,0xff,0xff,
		0xff,0xb5,0x10,0xfd,0xff,0xff,
		0x53
	};
	ULONG nKDETagLen = sizeof(pKDETag);

	if (!KeServiceDescriptorTable)
	{
		return NULL;
	}

	ServiceTableBase = KeServiceDescriptorTable->ServiceTableBase;
	pNtRaiseException = ServiceTableBase[0xB5];

	pCallAddr = SearchAddressBySig(pNtRaiseException, 0x100, pKRETag, nKRETagLen);
	if (!pCallAddr)
	{
		return NULL;
	}
	pCallAddr += nKRETagLen;

	nCallVal = *(ULONG*)(pCallAddr + 1);
	pKiRaiseException = pCallAddr + nCallVal + 5;

	pCallAddr = SearchAddressBySig(pKiRaiseException, 0x1000, pKDETag, nKDETagLen);
	if (!pCallAddr)
	{
		return NULL;
	}
	pCallAddr += nKDETagLen;

	nCallVal = *(ULONG*)(pCallAddr + 1);
	pKiDispatchException = pCallAddr + nCallVal + 5;

	return pKiDispatchException;

}
//////////////////////////////////////////////////////////////////////////
typedef NTSTATUS (NTAPI *pfuncNtQueryInformationProcess)(
	IN HANDLE ProcessHandle, 
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation, 
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL);

pfuncNtQueryInformationProcess g_OldNtQueryInformationProcess = NULL;
NTSTATUS NewNtQueryInformationProcess(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL)
{
	if (IsTargetProcess())
	{
		DEBUG_PRINT("ProcHandle: %d Class: %d\n", ProcessHandle, ProcessInformationClass);
	}

	return (pfuncNtQueryInformationProcess)g_OldNtQueryInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
}

typedef NTSTATUS (NTAPI *pfuncNtQueryObject)(IN HANDLE ObjectHandle, 
	IN OBJECT_INFORMATION_CLASS ObjectInformationClass,
	OUT PVOID ObjectInformation, 
	IN ULONG ObjectInformationLength, 
	OUT PULONG ReturnLength OPTIONAL);
pfuncNtQueryObject g_OldNtQueryObject = NULL;
NTSTATUS NewNtQueryObject(IN HANDLE ObjectHandle, IN OBJECT_INFORMATION_CLASS ObjectInformationClass, OUT PVOID ObjectInformation, IN ULONG ObjectInformationLength, OUT PULONG ReturnLength OPTIONAL)
{
	if (IsTargetProcess())
	{
		DEBUG_PRINT("ObjectHandle: %d Class: %d\n", ObjectHandle, ObjectInformationClass);
	}

	return (pfuncNtQueryObject)g_OldNtQueryObject(ObjectHandle, ObjectInformationClass, ObjectInformation, ObjectInformationLength, ReturnLength);
}

typedef VOID
(NTAPI *pfuncKiDispatchException) (
	IN PEXCEPTION_RECORD ExceptionRecord,
	IN PKEXCEPTION_FRAME ExceptionFrame,
	IN PKTRAP_FRAME TrapFrame,
	IN KPROCESSOR_MODE PreviousMode,
	IN BOOLEAN FirstChance);
pfuncKiDispatchException KiDispatchException = NULL;
pfuncKiDispatchException g_OldKiDispatchException = NULL;
ULONG nKDEPatchLen = 0;
VOID NewKiDispatchException(
	IN PEXCEPTION_RECORD ExceptionRecord,
	IN PKEXCEPTION_FRAME ExceptionFrame,
	IN PKTRAP_FRAME TrapFrame,
	IN KPROCESSOR_MODE PreviousMode,
	IN BOOLEAN FirstChance)
{
	if (PreviousMode == UserMode && IsTargetProcess())
	{
		DEBUG_PRINT("User Exception Code: %08X", ExceptionRecord->ExceptionCode);
	}
	return g_OldKiDispatchException(ExceptionRecord, ExceptionFrame, TrapFrame, PreviousMode, FirstChance);
}
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

	//if (!RtlCompareUnicodeString(&usTargetName, pusImageName, TRUE))
	//{
	//	DEBUG_PRINT("TargetProcess launched: %wZ", &szImageName)
	//	g_hTargetId = ProcessId;
	//}

	if (pusImageName)
	{
		ExFreePool(pusImageName);
	}
}


VOID DriverUnload(PDRIVER_OBJECT pDriverObj)
{
	KernelInlineUnHook(KiDispatchException, g_OldKiDispatchException, nKDEPatchLen);
	KernelInlineHookDrop();

	UnHookSSDT(SN_NtQueryInformationProcess, g_OldNtQueryInformationProcess);
	UnHookSSDT(SN_NtQueryObject, g_OldNtQueryObject);

	PsSetCreateProcessNotifyRoutine(CreateProcessNotify, TRUE);
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegistryString)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING ustrDevName;
	void* pFunc = NULL;

	pDriverObj->DriverUnload = DriverUnload;

	InitUndocFunctions();

	PsSetCreateProcessNotifyRoutine(CreateProcessNotify, FALSE);


	HookSSDT(SN_NtQueryInformationProcess, NewNtQueryInformationProcess, &g_OldNtQueryInformationProcess);
	HookSSDT(SN_NtQueryObject, NewNtQueryObject, &g_OldNtQueryObject);

	//加载HOOK
	KernelInlineHookInit();

	KiDispatchException = FindKiDispatchException();
	if (KiDispatchException)
	{
		KernelInlineHook(KiDispatchException, NewKiDispatchException, &g_OldKiDispatchException, &nKDEPatchLen);
	}

	//初始化SSDT相关函数

	return STATUS_SUCCESS;
}