#ifndef TOOLS_H
#define TOOLS_H


ULONG_PTR SearchAddressBySig(ULONG_PTR BaseAddr, ULONG Range, PUCHAR Sig, ULONG SigLen);

ULONG_PTR FindFuntion(PWCHAR FuncName);

NTSTATUS SafeCopyMemory(PVOID DstAddr, PVOID SrcAddr, ULONG Size);

WCHAR* UsStr(PUNICODE_STRING pusStr1, PUNICODE_STRING pusStr2);
//////////////////////////////////////////////////////////////////////////



typedef NTSTATUS(*pfuncZwQueryInformationProcess) (
	__in HANDLE ProcessHandle,
	__in PROCESSINFOCLASS ProcessInformationClass,
	__out_bcount(ProcessInformationLength) PVOID ProcessInformation,
	__in ULONG ProcessInformationLength,
	__out_opt PULONG ReturnLength
	);
pfuncZwQueryInformationProcess ZwQueryInformationProcess;


BOOLEAN InitUndocFunctions();

#endif