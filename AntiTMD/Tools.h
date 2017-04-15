#ifndef TOOLS_H
#define TOOLS_H


void DebugPrint(const char* function, const char* format, ...);

#define DEBUG_PRINT(format, ...) DebugPrint(__FUNCTION__, format, __VA_ARGS__);


VOID* SearchAddressBySig(ULONG_PTR BaseAddr, ULONG Range, PUCHAR Sig, ULONG SigLen);

ULONG_PTR FindFuntion(PWCHAR FuncName);

NTSTATUS SafeCopyMemory(PVOID DstAddr, PVOID SrcAddr, ULONG Size);

WCHAR* UsStr(PUNICODE_STRING pusStr1, PUNICODE_STRING pusStr2);

PUNICODE_STRING GetProcNameByEproc(IN PEPROCESS pEproc);

VOID* FindKiDispatchException();
//////////////////////////////////////////////////////////////////////////



#endif