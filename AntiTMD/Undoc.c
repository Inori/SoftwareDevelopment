#include "Undoc.h"
#include "SSDTHook.h"

ZWQUERYINFORMATIONPROCESS ZwQueryInformationProcess = 0;
ZWQUERYINFORMATIONTHREAD ZwQueryInformationThread = 0;
//NTQUERYOBJECT NtQueryObject = 0;
ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation = 0;
NTQUERYSYSTEMINFORMATION NtQuerySystemInformation = 0;
//NTCLOSE NtClose = 0;
NTSETCONTEXTTHREAD NtSetContextThread = 0;
NTCONTINUE NtContinue = 0;
NTDUPLICATEOBJECT NtDuplicateObject = 0;
KERAISEUSEREXCEPTION KeRaiseUserException = 0;
//NTSETINFORMATIONTHREAD NtSetInformationThread = 0;
NTSETINFORMATIONPROCESS NtSetInformationProcess = 0;
//NTQUERYINFORMATIONPROCESS NtQueryInformationProcess = 0;
NTSYSTEMDEBUGCONTROL NtSystemDebugControl = 0;

//////////////////////////////////////////////////////////////////////////
BOOLEAN InitUndocFunctions()
{
	if (!ZwQueryInformationProcess)
	{
		UNICODE_STRING routineName;
		RtlInitUnicodeString(&routineName, L"ZwQueryInformationProcess");
		ZwQueryInformationProcess = (ZWQUERYINFORMATIONPROCESS)MmGetSystemRoutineAddress(&routineName);
		if (!ZwQueryInformationProcess)
			return FALSE;
	}
	if (!ZwQueryInformationThread)
	{
		UNICODE_STRING routineName;
		RtlInitUnicodeString(&routineName, L"ZwQueryInformationThread");
		ZwQueryInformationThread = (ZWQUERYINFORMATIONTHREAD)MmGetSystemRoutineAddress(&routineName);
		if (!ZwQueryInformationThread)
			return FALSE;
	}
	if (!ZwQuerySystemInformation)
	{
		UNICODE_STRING routineName;
		RtlInitUnicodeString(&routineName, L"ZwQuerySystemInformation");
		ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)MmGetSystemRoutineAddress(&routineName);
		if (!ZwQuerySystemInformation)
			return FALSE;
	}
	if (!NtQuerySystemInformation)
	{
		UNICODE_STRING routineName;
		RtlInitUnicodeString(&routineName, L"NtQuerySystemInformation");
		NtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)MmGetSystemRoutineAddress(&routineName);
		if (!NtQuerySystemInformation)
			return FALSE;
	}
	//if (!NtClose)
	//{
	//	UNICODE_STRING routineName;
	//	RtlInitUnicodeString(&routineName, L"NtClose");
	//	NtClose = (NTCLOSE)MmGetSystemRoutineAddress(&routineName);
	//	if (!NtClose)
	//		return FALSE;
	//}
	if (!NtDuplicateObject)
	{
		UNICODE_STRING routineName;
		RtlInitUnicodeString(&routineName, L"NtDuplicateObject");
		NtDuplicateObject = (NTDUPLICATEOBJECT)MmGetSystemRoutineAddress(&routineName);
		if (!NtDuplicateObject)
			return FALSE;
	}
	if (!KeRaiseUserException)
	{
		UNICODE_STRING routineName;
		RtlInitUnicodeString(&routineName, L"KeRaiseUserException");
		KeRaiseUserException = (KERAISEUSEREXCEPTION)MmGetSystemRoutineAddress(&routineName);
		if (!KeRaiseUserException)
			return FALSE;
	}
	//if (!NtSetInformationThread)
	//{
	//	UNICODE_STRING routineName;
	//	RtlInitUnicodeString(&routineName, L"NtSetInformationThread");
	//	NtSetInformationThread = (NTSETINFORMATIONTHREAD)MmGetSystemRoutineAddress(&routineName);
	//	if (!NtSetInformationThread)
	//		return FALSE;
	//}
	if (!NtSetInformationProcess)
	{
		UNICODE_STRING routineName;
		RtlInitUnicodeString(&routineName, L"NtSetInformationProcess");
		NtSetInformationProcess = (NTSETINFORMATIONPROCESS)MmGetSystemRoutineAddress(&routineName);
		if (!NtSetInformationProcess)
			return FALSE;
	}
	//if (!NtQueryInformationProcess)
	//{
	//	UNICODE_STRING routineName;
	//	RtlInitUnicodeString(&routineName, L"NtQueryInformationProcess");
	//	NtQueryInformationProcess = (NTQUERYINFORMATIONPROCESS)MmGetSystemRoutineAddress(&routineName);
	//	if (!NtQueryInformationProcess)
	//		return FALSE;
	//}
	//SSDT-only functions after this
	//if (!NtQueryObject)
	//{
	//	NtQueryObject = (NTQUERYOBJECT)GetSSDTFuncAddr(SN_NtQueryObject);
	//	if (!NtQueryObject)
	//		return FALSE;
	//}
	if (!NtSetContextThread)
	{
		NtSetContextThread = (NTSETCONTEXTTHREAD)GetSSDTFuncAddr(SN_NtSetContextThread);
		if (!NtSetContextThread)
			return FALSE;
	}
	if (!NtContinue)
	{
		NtContinue = (NTCONTINUE)GetSSDTFuncAddr(SN_NtContinue);
		if (!NtContinue)
			return FALSE;
	}
	if (!NtSystemDebugControl)
	{
		NtSystemDebugControl = (NTSYSTEMDEBUGCONTROL)GetSSDTFuncAddr(SN_NtSystemDebugControl);
		if (!NtSystemDebugControl)
			return FALSE;
	}
	return TRUE;
}
