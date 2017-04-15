#include "HookFuncs.h"
#include "Undoc.h"
#include "KernelInlineHook.h"
#include "SSDTHook.h"
#include "Tools.h"

//////////////////////////////////////////////////////////////////////////
HANDLE g_hTargetId = 0;


//////////////////////////////////////////////////////////////////////////


BOOLEAN IsTargetProcess()
{
	HANDLE hPid = 0;

	hPid = PsGetCurrentProcessId();

	return (hPid == g_hTargetId);
}
//////////////////////////////////////////////////////////////////////////
NTQUERYINFORMATIONPROCESS g_OldNtQueryInformationProcess = NULL;
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

	return (NTQUERYINFORMATIONPROCESS)g_OldNtQueryInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
}

NTQUERYOBJECT g_OldNtQueryObject = NULL;
NTSTATUS NewNtQueryObject(IN HANDLE ObjectHandle, IN UNDOC_OBJECT_INFORMATION_CLASS ObjectInformationClass, OUT PVOID ObjectInformation, IN ULONG ObjectInformationLength, OUT PULONG ReturnLength OPTIONAL)
{
	if (IsTargetProcess())
	{
		DEBUG_PRINT("ObjectHandle: %d Class: %d\n", ObjectHandle, ObjectInformationClass);
	}

	return (NTQUERYOBJECT)g_OldNtQueryObject(ObjectHandle, ObjectInformationClass, ObjectInformation, ObjectInformationLength, ReturnLength);
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

//////////////////////////////////////////////////////////////////////////

VOID InstallHooks()
{
	HookSSDT(SN_NtQueryInformationProcess, NewNtQueryInformationProcess, &g_OldNtQueryInformationProcess);
	HookSSDT(SN_NtQueryObject, NewNtQueryObject, &g_OldNtQueryObject);

	//º”‘ÿHOOK
	KernelInlineHookInit();

	KiDispatchException = FindKiDispatchException();
	if (KiDispatchException)
	{
		KernelInlineHook(KiDispatchException, NewKiDispatchException, &g_OldKiDispatchException, &nKDEPatchLen);
	}
}

VOID UnInstallHooks()
{
	KernelInlineUnHook(KiDispatchException, g_OldKiDispatchException, nKDEPatchLen);
	KernelInlineHookDrop();

	UnHookSSDT(SN_NtQueryInformationProcess, g_OldNtQueryInformationProcess);
	UnHookSSDT(SN_NtQueryObject, g_OldNtQueryObject);
}