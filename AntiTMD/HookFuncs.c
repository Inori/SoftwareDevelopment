#include "HookFuncs.h"
#include "Undoc.h"
#include "KernelInlineHook.h"
#include "SSDTHook.h"
#include "Tools.h"

//////////////////////////////////////////////////////////////////////////
FAST_MUTEX g_DebugPortMutex;

HANDLE g_hTargetId = 0;

BOOLEAN IsTargetProcess()
{
	HANDLE hPid = 0;

	hPid = PsGetCurrentProcessId();

	return (hPid == g_hTargetId);
}
//////////////////////////////////////////////////////////////////////////



void FilterObject(POBJECT_TYPE_INFORMATION pObject)
{
	const WCHAR strDebugObject[] = L"DebugObject";

	if (pObject->TypeName.Length == 0 || pObject->TypeName.Buffer == 0)
	{
		return;
	}

	if (pObject->TypeName.Length == (sizeof(strDebugObject) - sizeof(WCHAR)))
	{
		if (!memcmp(strDebugObject, pObject->TypeName.Buffer, pObject->TypeName.Length))
		{
			pObject->TotalNumberOfObjects = 0;
			pObject->TotalNumberOfHandles = 0;
		}
	}

}

void FilterObjects(POBJECT_TYPES_INFORMATION pObjectTypes)
{
	POBJECT_TYPE_INFORMATION pObject = pObjectTypes->TypeInformation;
	ULONG i = 0;
	for (i = 0; i < pObjectTypes->NumberOfTypes; i++)
	{
		FilterObject(pObject);

		pObject = (POBJECT_TYPE_INFORMATION)(((PCHAR)(pObject + 1) + ALIGN_UP(pObject->TypeName.MaximumLength, ULONG_PTR)));
	}
}


void FakeCurrentParentProcessId(PSYSTEM_PROCESS_INFORMATION pInfo)
{
	const WCHAR ExplorerProcessName[] = L"explorer.exe";
	static LONG nExplorerPid = 0;
	if (!nExplorerPid)
	{
		const USHORT explorerNameLength = (USHORT)wcslen(ExplorerProcessName);
		PSYSTEM_PROCESS_INFORMATION pTemp = pInfo;
		while (TRUE)
		{
			if (pTemp->ImageName.Buffer && pTemp->ImageName.Length)
			{
				if (pTemp->ImageName.Length == explorerNameLength)
				{
					if (!_wcsnicmp(pTemp->ImageName.Buffer, ExplorerProcessName, pTemp->ImageName.Length))
					{
						nExplorerPid = (LONG)pTemp->UniqueProcessId;
						break;
					}
				}
			}

			if (pTemp->NextEntryOffset == 0)
			{
				break;
			}
			else
			{
				pTemp = (PSYSTEM_PROCESS_INFORMATION)((DWORD_PTR)pTemp + pTemp->NextEntryOffset);
			}
		}
	}

	if (nExplorerPid)
	{
		while (TRUE)
		{
			if (pInfo->UniqueProcessId == (HANDLE)PsGetCurrentProcessId())
			{
				pInfo->InheritedFromUniqueProcessId = (HANDLE)nExplorerPid;
				break;
			}

			if (pInfo->NextEntryOffset == 0)
			{
				break;
			}
			else
			{
				pInfo = (PSYSTEM_PROCESS_INFORMATION)((DWORD_PTR)pInfo + pInfo->NextEntryOffset);
			}
		}
	}
}

void FilterProcess(PSYSTEM_PROCESS_INFORMATION pInfo)
{
	PSYSTEM_PROCESS_INFORMATION pPrev = pInfo;

	while (TRUE)
	{
		if (IsProcessBad(&pInfo->ImageName) || IsTargetProcess())
		{
			if (pInfo->ImageName.Buffer)
				RtlZeroMemory(pInfo->ImageName.Buffer, pInfo->ImageName.Length);

			if (pInfo->NextEntryOffset == 0) //last element
			{
				pPrev->NextEntryOffset = 0;
			}
			else
			{
				pPrev->NextEntryOffset += pInfo->NextEntryOffset;
			}
		}
		else
		{
			pPrev = pInfo;
		}

		if (pInfo->NextEntryOffset == 0)
		{
			break;
		}
		else
		{
			pInfo = (PSYSTEM_PROCESS_INFORMATION)((DWORD_PTR)pInfo + pInfo->NextEntryOffset);
		}
	}
}
//////////////////////////////////////////////////////////////////////////

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
	ULONG ExceptionCode = 0;
	ULONG IsHWBP = 0;
	if (PreviousMode == UserMode && IsTargetProcess())
	{
		ExceptionCode = ExceptionRecord->ExceptionCode;
		IsHWBP = TrapFrame->Dr6 & 0x0F;
	
		if (ExceptionCode == STATUS_SINGLE_STEP && IsHWBP)
		{
			DEBUG_PRINT("HWBP DR0: %08X, DR1: %08X", TrapFrame->Dr0, TrapFrame->Dr1);
			//DbgBreakPoint();
		}
	}
	return g_OldKiDispatchException(ExceptionRecord, ExceptionFrame, TrapFrame, PreviousMode, FirstChance);
}

VOID HookKiDispatchException()
{
	KiDispatchException = FindKiDispatchException();
	if (KiDispatchException)
	{
		KernelInlineHook(KiDispatchException, NewKiDispatchException, &g_OldKiDispatchException, &nKDEPatchLen);
	}
}

VOID* KiSystemService = (VOID*)0x8054261A;
VOID* g_OldKiSystemService = NULL;
ULONG nKSSPatchLen = 0;
VOID __declspec(naked) NewKiSystemService()
{
	__asm
	{
		//pushad
		//pushfd
		//popfd
		//popad
		jmp g_OldKiSystemService
	}
}

//////////////////////////////////////////////////////////////////////////
NTQUERYOBJECT g_OldNtQueryObject = NULL;
NTSTATUS NewNtQueryObject(IN HANDLE ObjectHandle, IN UNDOC_OBJECT_INFORMATION_CLASS ObjectInformationClass, OUT PVOID ObjectInformation, IN ULONG ObjectInformationLength, OUT PULONG ReturnLength OPTIONAL)
{
	NTSTATUS status;
	status = (NTQUERYOBJECT)g_OldNtQueryObject(ObjectHandle, ObjectInformationClass, ObjectInformation, ObjectInformationLength, ReturnLength);
	if (NT_SUCCESS(status) && ObjectInformation && IsTargetProcess())
	{
		DEBUG_PRINT("Class: %d\n", ObjectInformationClass);
		if (ObjectInformationClass == UndocObjectTypesInformation)
		{
			FilterObjects((POBJECT_TYPES_INFORMATION)ObjectInformation);
		}
		else if (ObjectInformationClass == UndocObjectTypeInformation)
		{
			FilterObject((POBJECT_TYPE_INFORMATION)ObjectInformation);
		}
	}

	return status;
}

NTQUERYSYSTEMINFORMATION g_OldNtQuerySystemInformation = NULL;
NTSTATUS NTAPI NewNtQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	)
{
	NTSTATUS status;
	status = (NTQUERYINFORMATIONPROCESS)g_OldNtQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
	if (NT_SUCCESS(status) && SystemInformation)
	{
		if (IsTargetProcess())
		{
			if (SystemInformationClass == SystemKernelDebuggerInformation)
			{
				SYSTEM_KERNEL_DEBUGGER_INFORMATION* DebuggerInfo = (SYSTEM_KERNEL_DEBUGGER_INFORMATION*)SystemInformation;
				DEBUG_PRINT("SystemKernelDebuggerInformation\n");
				__try
				{
					DebuggerInfo->DebuggerEnabled = FALSE;
					DebuggerInfo->DebuggerNotPresent = TRUE;
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					status = GetExceptionCode();
				}
			}
		}
	}

	return status;
}

NTCLOSE g_OldNtClose = NULL;
NTSTATUS NTAPI NewNtClose(IN HANDLE Handle)
{
	PVOID OldDebugPort = NULL;
	NTSTATUS status;
	if (IsTargetProcess())
	{
		ExAcquireFastMutex(&g_DebugPortMutex);
		OldDebugPort = SetDebugPort(PsGetCurrentProcess(), 0);
		status = (NTCLOSE)g_OldNtClose(Handle);
		SetDebugPort(PsGetCurrentProcess(), OldDebugPort);
		ExReleaseFastMutex(&g_DebugPortMutex);
		if (!NT_SUCCESS(status))
			DEBUG_PRINT("NtClose(0x%p)\n", Handle);
	}
	else
	{
		status = (NTCLOSE)g_OldNtClose(Handle);
	}
	return status;
}

NTSETCONTEXTTHREAD g_OldNtSetContextThread = NULL;
NTSTATUS NTAPI NewNtSetContextThread(
	IN HANDLE ThreadHandle,
	IN PCONTEXT Context
)
{
	NTSTATUS status;
	ULONG OriginalContextFlags = 0;
	ULONG NewContextFlags = 0;
	BOOLEAN IsTarget = FALSE;

	IsTarget = IsTargetProcess();
	if (IsTarget)
	{
		OriginalContextFlags = Context->ContextFlags;
		NewContextFlags = OriginalContextFlags & ~0x10; //CONTEXT_DEBUG_REGISTERS ^ CONTEXT_AMD64/CONTEXT_i386
		Context->ContextFlags = NewContextFlags;
	}
	status = (NTSETCONTEXTTHREAD)g_OldNtSetContextThread(ThreadHandle, Context);
	if (IsTarget)
	{
		Context->ContextFlags = OriginalContextFlags;
	}
	return status;
}

NTGETCONTEXTTHREAD g_OldNtGetContextThread = NULL;
NTSTATUS NTAPI NewNtGetContextThread(
	IN HANDLE ThreadHandle,
	IN PCONTEXT Context
)
{
	NTSTATUS status;
	ULONG OriginalContextFlags = 0;
	ULONG NewContextFlags = 0;
	BOOLEAN IsTarget = FALSE;

	IsTarget = IsTargetProcess();
	if (IsTarget)
	{
		OriginalContextFlags = Context->ContextFlags;
		NewContextFlags = OriginalContextFlags & ~0x10; //CONTEXT_DEBUG_REGISTERS ^ CONTEXT_AMD64/CONTEXT_i386
		Context->ContextFlags = NewContextFlags;
	}
	status = (NTGETCONTEXTTHREAD)g_OldNtGetContextThread(ThreadHandle, Context);
	if (IsTarget)
	{
		Context->ContextFlags = OriginalContextFlags;
	}
	return status;
}

NTCONTINUE g_OldNtContinue = NULL;
NTSTATUS NTAPI NewNtContinue(
	IN PCONTEXT Context,
	BOOLEAN RaiseAlert
)
{
	if (IsTargetProcess())
	{
		//DEBUG_PRINT("Contex Flag: %X\n", Context->ContextFlags);
	}
	return (NTCONTINUE)g_OldNtContinue(Context, RaiseAlert);
}

NTDUPLICATEOBJECT g_OldNtDuplicateObject = NULL;
NTSTATUS NTAPI NewNtDuplicateObject(
	IN HANDLE SourceProcessHandle,
	IN HANDLE SourceHandle,
	IN HANDLE TargetProcessHandle,
	OUT PHANDLE TargetHandle,
	IN ACCESS_MASK DesiredAccess OPTIONAL,
	IN ULONG HandleAttributes,
	IN ULONG Options
)
{
	if (IsTargetProcess())
	{
		//DEBUG_PRINT("Hit\n");
	}
	return (NTDUPLICATEOBJECT)g_OldNtDuplicateObject(SourceProcessHandle, SourceHandle, TargetProcessHandle, TargetHandle, DesiredAccess, HandleAttributes, Options);
}

NTSETINFORMATIONTHREAD g_OldNtSetInformationThread = NULL;
NTSTATUS NTAPI NewNtSetInformationThread(
	IN HANDLE ThreadHandle,
	IN THREADINFOCLASS ThreadInformationClass,
	IN PVOID ThreadInformation,
	IN ULONG ThreadInformationLength
)
{
	if (IsTargetProcess())
	{
		if (ThreadInformationClass == ThreadHideFromDebugger && !ThreadInformationLength)
		{
			//PETHREAD Thread;
			//NTSTATUS status;
			DEBUG_PRINT("ThreadHideFromDebugger\n");
	
			//status = ObReferenceObjectByHandle(ThreadHandle,
			//	THREAD_SET_INFORMATION,
			//	*PsThreadType,
			//	ExGetPreviousMode(),
			//	(PVOID*)&Thread,
			//	NULL);
			//if (NT_SUCCESS(status))
			//{
			//	ObDereferenceObject(Thread);
			//}
			//return status;
			return STATUS_SUCCESS;
		}
	}
	return (NTSETINFORMATIONTHREAD)g_OldNtSetInformationThread(ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength);
}


static ULONG ValueProcessBreakOnTermination = FALSE;
static BOOLEAN IsProcessHandleTracingEnabled = FALSE;

NTSETINFORMATIONPROCESS g_OldNtSetInformationProcess = NULL;
NTSTATUS NTAPI NewNtSetInformationProcess(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	IN PVOID ProcessInformation,
	IN ULONG ProcessInformationLength
)
{
	if (IsTargetProcess())
	{
		//DEBUG_PRINT("ProcessInformationClass: %d\n", ProcessInformationClass);
	}
	return (NTSETINFORMATIONPROCESS)g_OldNtSetInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength);
}


NTQUERYINFORMATIONPROCESS g_OldNtQueryInformationProcess = NULL;
NTSTATUS NewNtQueryInformationProcess(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL)
{
	NTSTATUS status;
	status = (NTQUERYINFORMATIONPROCESS)g_OldNtQueryInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
	if (NT_SUCCESS(status) && IsTargetProcess())
	{
		DEBUG_PRINT("ProcHandle: %d Class: %d\n", ProcessHandle, ProcessInformationClass);
		if (ProcessInformation != 0 && ProcessInformationLength != 0)
		{
			if (ProcessInformationClass == ProcessDebugFlags)
			{
				*((ULONG *)ProcessInformation) = 1;
			}
			else if (ProcessInformationClass == ProcessDebugObjectHandle)
			{
				*((HANDLE *)ProcessInformation) = 0;
				return STATUS_PORT_NOT_SET;
			}
			else if (ProcessInformationClass == ProcessDebugPort)
			{
				*((HANDLE *)ProcessInformation) = 0;
			}
			else if (ProcessInformationClass == ProcessBasicInformation) //Fake parent
			{
				DEBUG_PRINT("ProcessBasicInformation\n");
				//((PPROCESS_BASIC_INFORMATION)ProcessInformation)->InheritedFromUniqueProcessId = (HANDLE)GetExplorerProcessId();
			}
			else if (ProcessInformationClass == ProcessBreakOnTermination)
			{
				*((ULONG *)ProcessInformation) = ValueProcessBreakOnTermination;
			}
			else if (ProcessInformationClass == ProcessHandleTracing)
			{
				if (IsProcessHandleTracingEnabled)
				{
					return STATUS_SUCCESS;
				}
				else
				{
					return STATUS_INVALID_PARAMETER;
				}
			}
		}
	}

	return status;
}

NTSYSTEMDEBUGCONTROL g_OldNtSystemDebugControl = NULL;
NTSTATUS NTAPI NewNtSystemDebugControl(
	IN SYSDBG_COMMAND Command,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength,
	OUT PULONG ReturnLength OPTIONAL
)
{
	if (IsTargetProcess())
	{
		//DEBUG_PRINT("Command: %d Class: %d\n", Command);
	}
	return (NTSYSTEMDEBUGCONTROL)g_OldNtSystemDebugControl(Command, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength, ReturnLength);
}
//////////////////////////////////////////////////////////////////////////

VOID InstallHooks()
{
	ExInitializeFastMutex(&g_DebugPortMutex);

	//HookSSDT(SN_NtQueryObject, NewNtQueryObject, &g_OldNtQueryObject);
	//HookSSDT(SN_NtQuerySystemInformation, NewNtQuerySystemInformation, &g_OldNtQuerySystemInformation);
	//HookSSDT(SN_NtClose, NewNtClose, &g_OldNtClose);
	HookSSDT(SN_NtSetContextThread, NewNtSetContextThread, &g_OldNtSetContextThread);
	HookSSDT(SN_NtGetContextThread, NewNtGetContextThread, &g_OldNtGetContextThread);
	//HookSSDT(SN_NtContinue, NewNtContinue, &g_OldNtContinue);
	//HookSSDT(SN_NtDuplicateObject, NewNtDuplicateObject, &g_OldNtDuplicateObject);
	//HookSSDT(SN_NtSetInformationProcess, NewNtSetInformationProcess, &g_OldNtSetInformationProcess);
	HookSSDT(SN_NtSetInformationThread, NewNtSetInformationThread, &g_OldNtSetInformationThread);
	HookSSDT(SN_NtQueryInformationProcess, NewNtQueryInformationProcess, &g_OldNtQueryInformationProcess);
	//HookSSDT(SN_NtSystemDebugControl, NewNtSystemDebugControl, &g_OldNtSystemDebugControl);
	//º”‘ÿHOOK
	KernelInlineHookInit();
	HookKiDispatchException();

	//KernelInlineHook(KiSystemService, NewKiSystemService, &g_OldKiSystemService, &nKSSPatchLen);
}

VOID UnInstallHooks()
{
	//KernelInlineUnHook(KiSystemService, g_OldKiSystemService, nKSSPatchLen);
	KernelInlineUnHook(KiDispatchException, g_OldKiDispatchException, nKDEPatchLen);
	KernelInlineHookDrop();

	//UnHookSSDT(SN_NtQueryObject, g_OldNtQueryObject);
	//UnHookSSDT(SN_NtQuerySystemInformation, g_OldNtQuerySystemInformation);
	//UnHookSSDT(SN_NtClose, g_OldNtClose);
	UnHookSSDT(SN_NtSetContextThread, g_OldNtSetContextThread);
	UnHookSSDT(SN_NtGetContextThread, g_OldNtGetContextThread);
	//UnHookSSDT(SN_NtContinue, g_OldNtContinue);
	//UnHookSSDT(SN_NtDuplicateObject, g_OldNtDuplicateObject);
	//UnHookSSDT(SN_NtSetInformationProcess, g_OldNtSetInformationProcess);
	UnHookSSDT(SN_NtSetInformationThread, g_OldNtSetInformationThread);
	UnHookSSDT(SN_NtQueryInformationProcess, g_OldNtQueryInformationProcess);
	//UnHookSSDT(SN_NtSystemDebugControl, g_OldNtSystemDebugControl);
}