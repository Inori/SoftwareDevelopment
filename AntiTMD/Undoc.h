#pragma once
#include <ntddk.h>

typedef struct _KTRAP_FRAME {


	//
	//  Following 4 values are only used and defined for DBG systems,
	//  but are always allocated to make switching from DBG to non-DBG
	//  and back quicker.  They are not DEVL because they have a non-0
	//  performance impact.
	//

	ULONG   DbgEbp;         // Copy of User EBP set up so KB will work.
	ULONG   DbgEip;         // EIP of caller to system call, again, for KB.
	ULONG   DbgArgMark;     // Marker to show no args here.
	ULONG   DbgArgPointer;  // Pointer to the actual args

							//
							//  Temporary values used when frames are edited.
							//
							//
							//  NOTE:   Any code that want's ESP must materialize it, since it
							//          is not stored in the frame for kernel mode callers.
							//
							//          And code that sets ESP in a KERNEL mode frame, must put
							//          the new value in TempEsp, make sure that TempSegCs holds
							//          the real SegCs value, and put a special marker value into SegCs.
							//

	ULONG   TempSegCs;
	ULONG   TempEsp;

	//
	//  Debug registers.
	//

	ULONG   Dr0;
	ULONG   Dr1;
	ULONG   Dr2;
	ULONG   Dr3;
	ULONG   Dr6;
	ULONG   Dr7;

	//
	//  Segment registers
	//

	ULONG   SegGs;
	ULONG   SegEs;
	ULONG   SegDs;

	//
	//  Volatile registers
	//

	ULONG   Edx;
	ULONG   Ecx;
	ULONG   Eax;

	//
	//  Nesting state, not part of context record
	//

	ULONG   PreviousPreviousMode;

	VOID* ExceptionList;
	// Trash if caller was user mode.
	// Saved exception list if caller
	// was kernel mode or we're in
	// an interrupt.

	//
	//  FS is TIB/PCR pointer, is here to make save sequence easy
	//

	ULONG   SegFs;

	//
	//  Non-volatile registers
	//

	ULONG   Edi;
	ULONG   Esi;
	ULONG   Ebx;
	ULONG   Ebp;

	//
	//  Control registers
	//

	ULONG   ErrCode;
	ULONG   Eip;
	ULONG   SegCs;
	ULONG   EFlags;

	ULONG   HardwareEsp;    // WARNING - segSS:esp are only here for stacks
	ULONG   HardwareSegSs;  // that involve a ring transition.

	ULONG   V86Es;          // these will be present for all transitions from
	ULONG   V86Ds;          // V86 mode
	ULONG   V86Fs;
	ULONG   V86Gs;
} KTRAP_FRAME;

typedef KTRAP_FRAME *PKTRAP_FRAME;
typedef KTRAP_FRAME *PKEXCEPTION_FRAME;

//structures
typedef struct _OBJECT_TYPE_INFORMATION
{
	UNICODE_STRING TypeName;
	ULONG TotalNumberOfHandles;
	ULONG TotalNumberOfObjects;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

typedef struct _OBJECT_ALL_INFORMATION
{
	ULONG NumberOfObjects;
	OBJECT_TYPE_INFORMATION ObjectTypeInformation[1];
} OBJECT_ALL_INFORMATION, *POBJECT_ALL_INFORMATION;

//enums
typedef enum _UNDOC_OBJECT_INFORMATION_CLASS
{
	UndocObjectTypeInformation = 2,
	UndocObjectTypesInformation = 3
} UNDOC_OBJECT_INFORMATION_CLASS, *PUNDOC_OBJECT_INFORMATION_CLASS;

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemModuleInformation = 11,
	SystemKernelDebuggerInformation = 35
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

typedef enum _SYSDBG_COMMAND
{
	SysDbgGetTriageDump = 29,
} SYSDBG_COMMAND, *PSYSDBG_COMMAND;

//////////////////////////////////////////////////////////////////////////
typedef NTSTATUS(NTAPI* ZWQUERYINFORMATIONPROCESS)(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef NTSTATUS(NTAPI* ZWQUERYINFORMATIONTHREAD)(
	IN HANDLE ThreadHandle,
	IN THREADINFOCLASS ThreadInformationClass,
	IN OUT PVOID ThreadInformation,
	IN ULONG ThreadInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef NTSTATUS(NTAPI* NTQUERYOBJECT)(
	IN HANDLE Handle OPTIONAL,
	IN UNDOC_OBJECT_INFORMATION_CLASS ObjectInformationClass,
	OUT PVOID ObjectInformation OPTIONAL,
	IN ULONG ObjectInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef NTSTATUS(NTAPI* ZWQUERYSYSTEMINFORMATION)(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef NTSTATUS(NTAPI* NTQUERYSYSTEMINFORMATION)(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef NTSTATUS(NTAPI* NTCLOSE)(
	IN HANDLE Handle
	);

typedef NTSTATUS(NTAPI* NTSETCONTEXTTHREAD)(
	IN HANDLE ThreadHandle,
	IN PCONTEXT Context
	);

typedef NTSTATUS(NTAPI* NTCONTINUE)(
	IN PCONTEXT Context,
	BOOLEAN RaiseAlert
	);

typedef NTSTATUS(NTAPI* NTDUPLICATEOBJECT)(
	IN HANDLE SourceProcessHandle,
	IN HANDLE SourceHandle,
	IN HANDLE TargetProcessHandle,
	OUT PHANDLE TargetHandle,
	IN ACCESS_MASK DesiredAccess OPTIONAL,
	IN ULONG HandleAttributes,
	IN ULONG Options
	);

typedef NTSTATUS(NTAPI* KERAISEUSEREXCEPTION)(
	IN NTSTATUS ExceptionCode
	);

typedef NTSTATUS(NTAPI* NTSETINFORMATIONTHREAD)(
	IN HANDLE ThreadHandle,
	IN THREADINFOCLASS ThreadInformationClass,
	IN PVOID ThreadInformation,
	IN ULONG ThreadInformationLength
	);

typedef NTSTATUS(NTAPI* NTSETINFORMATIONPROCESS)(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	IN PVOID ProcessInformation,
	IN ULONG ProcessInformationLength
	);

typedef NTSTATUS(NTAPI* NTQUERYINFORMATIONPROCESS)(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef NTSTATUS(NTAPI* NTSYSTEMDEBUGCONTROL)(
	IN SYSDBG_COMMAND Command,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength,
	OUT PULONG ReturnLength OPTIONAL
	);

extern ZWQUERYINFORMATIONPROCESS ZwQueryInformationProcess;
extern ZWQUERYINFORMATIONTHREAD ZwQueryInformationThread;
//extern NTQUERYOBJECT NtQueryObject;
extern ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation;
extern NTQUERYSYSTEMINFORMATION NtQuerySystemInformation;
//extern NTCLOSE NtClose;
extern NTSETCONTEXTTHREAD NtSetContextThread;
extern NTCONTINUE NtContinue;
extern NTDUPLICATEOBJECT NtDuplicateObject;
extern KERAISEUSEREXCEPTION KeRaiseUserException;
//extern NTSETINFORMATIONTHREAD NtSetInformationThread;
extern NTSETINFORMATIONPROCESS NtSetInformationProcess;
//extern NTQUERYINFORMATIONPROCESS NtQueryInformationProcess;
extern NTSYSTEMDEBUGCONTROL NtSystemDebugControl;

//////////////////////////////////////////////////////////////////////////

BOOLEAN InitUndocFunctions();
