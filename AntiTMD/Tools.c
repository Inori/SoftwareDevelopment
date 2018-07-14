#include <Ntifs.h>
#include <ntddk.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ntstrsafe.h>
#include "Tools.h"
#include "SSDTHook.h"
#include "Undoc.h"


#define MAX_PATH	260
#define ALLOC_TAG 'ANTI'

VOID* SearchAddressBySig(ULONG_PTR BaseAddr, ULONG Range, PUCHAR Sig, ULONG SigLen)
{
	ULONG Len = 0;
	VOID* RetAddr = NULL;

	if (!MmIsAddressValid(BaseAddr) || Range <= 0)
		return NULL;
	if (Sig == NULL || SigLen <= 0)
		return NULL;

	__try
	{
		for (Len = 0; Len < Range; ++Len)
		{
			if (!memcmp((PUCHAR)BaseAddr + Len, Sig, SigLen))
				break;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return NULL;
	}


	if (Len == Range)
		return NULL;

	RetAddr = BaseAddr + Len;

	if (!MmIsAddressValid(RetAddr))
		return NULL;

	return RetAddr;
	
}

ULONG_PTR FindFuntion(PWCHAR FuncName)
{
	UNICODE_STRING FunctionName;
	ULONG_PTR FunctionAddress;

	RtlInitUnicodeString(&FunctionName, FuncName);
	FunctionAddress = (ULONG_PTR)MmGetSystemRoutineAddress(&FunctionName);

	return FunctionAddress;
}

NTSTATUS SafeCopyMemory(PVOID DstAddr, PVOID SrcAddr, ULONG Size)
{
	PMDL  pSrcMdl, pDstMdl;
	PUCHAR pSrcAddress, pDstAddress;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	pSrcMdl = IoAllocateMdl(SrcAddr, Size, FALSE, FALSE, NULL);
	if (MmIsAddressValid(pSrcMdl))
	{
		MmBuildMdlForNonPagedPool(pSrcMdl);
		pSrcAddress = MmGetSystemAddressForMdlSafe(pSrcMdl, NormalPagePriority);

		if (MmIsAddressValid(pSrcAddress))
		{
			pDstMdl = IoAllocateMdl(DstAddr, Size, FALSE, FALSE, NULL);
			if (MmIsAddressValid(pDstMdl))
			{
				__try
				{
					MmProbeAndLockPages(pDstMdl, KernelMode, IoWriteAccess);
					pDstAddress = MmGetSystemAddressForMdlSafe(pDstMdl, NormalPagePriority);
					if (MmIsAddressValid(pDstAddress))
					{
						RtlZeroMemory(pDstAddress, Size);
						RtlCopyMemory(pDstAddress, pSrcAddress, Size);
						status = STATUS_SUCCESS;
					}
					MmUnlockPages(pDstMdl);
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					if (pDstMdl)
						MmUnlockPages(pDstMdl);

					if (pDstMdl)
						IoFreeMdl(pDstMdl);

					if (pSrcMdl)
						IoFreeMdl(pSrcMdl);

					return GetExceptionCode();
				}
				IoFreeMdl(pDstMdl);
			}
		}

		IoFreeMdl(pSrcMdl);
	}
	return status;
}


WCHAR* UsStr(PUNICODE_STRING pusStr1, PUNICODE_STRING pusStr2)
{
	WCHAR* pFindPos = NULL;
	ULONG nPos = 0;
	UCHAR* pStr1 = pusStr1->Buffer;
	UCHAR* pStr2 = pusStr2->Buffer;

	ULONG nStr1Len = pusStr1->Length;
	ULONG nStr2Len = pusStr2->Length;
	
	for (; nPos < nStr1Len; nPos += 2)
	{
		if (nStr1Len - nPos < nStr2Len)
		{
			break;
		}
		if (!memcmp(pStr1 + nPos, pStr2, nStr2Len))
		{
			pFindPos = pStr1 + nPos;
			break;
		}
	}

	return pFindPos;

}




//////////////////////////////////////////////////////////////////////////




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


void LogToFile(const char* function, const char* format, ...)
{
	char szTemp[DBG_STR_LEN] = { 0 };
	char szDbgStr[DBG_STR_LEN] = { 0 };
	UNICODE_STRING FileName;
	OBJECT_ATTRIBUTES objAttr;
	HANDLE handle;
	IO_STATUS_BLOCK ioStatusBlock;
	NTSTATUS ntstatus;

	va_list   arg_list;

	sprintf(szDbgStr, "Asuka->[%s]\t", function);

	va_start(arg_list, format);

	vsprintf(szTemp, format, arg_list);

	va_end(arg_list);

	strcat(szDbgStr, szTemp);
	strcat(szDbgStr, "\n");

	
	RtlInitUnicodeString(&FileName, L"\\DosDevices\\C:\\AntiTmd.log");
	InitializeObjectAttributes(&objAttr, &FileName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);
	if (KeGetCurrentIrql() != PASSIVE_LEVEL)
	{
#ifdef _DEBUG
		DbgPrint("[TITANHIDE] KeGetCurrentIrql != PASSIVE_LEVEL!\n");
#endif
		return;
	}

	ntstatus = ZwCreateFile(&handle,
		FILE_APPEND_DATA,
		&objAttr, &ioStatusBlock, NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0);
	if (NT_SUCCESS(ntstatus))
	{
		size_t cb;
		ntstatus = RtlStringCbLengthA(szDbgStr, sizeof(szDbgStr), &cb);
		if (NT_SUCCESS(ntstatus))
			ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock, szDbgStr, (ULONG)cb, NULL, NULL);
		ZwClose(handle);
	}
}

PUNICODE_STRING GetProcNameByEproc(IN PEPROCESS pEproc)
/*++

Routine Description:

��ȡָ�����̵�����������

Arguments:

pEproc - ָ�����̵�EPROCESS��ַ

Return Value:

�ɹ��򷵻ؽ�������ʧ�ܷ���NULL

Comments:

�ú������صĽ��������ɵ��������ͷţ�ExFreePool����
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

	pNtRaiseException = GetSSDTFuncAddr(SN_NtRaiseException);
	if (!pNtRaiseException)
	{
		return NULL;
	}

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

PVOID SetDebugPort(PEPROCESS Process, PVOID DebugPort)
{
	const int DebugPortOffset = 0x0bc;
	PVOID* pDebugPort = NULL;

	pDebugPort = (PVOID*)((unsigned char*)Process + DebugPortOffset);
	return InterlockedExchangePointer(pDebugPort, DebugPort);
}

const WCHAR * BadProcessnameList[] =
{
	L"ollydbg.exe",
	L"x64_dbg.exe",
	L"windbg.exe",
	L"ImportREC.exe",
};
const int BadProcessnameCount = 4;

BOOLEAN IsProcessBad(PUNICODE_STRING process)
{
	WCHAR nameCopy[400];
	int i = 0;

	if (!process || process->Length == 0 || !process->Buffer)
	{
		return FALSE;
	}

	memset(nameCopy, 0, sizeof(nameCopy));

	if (process->Length > (sizeof(nameCopy) - sizeof(WCHAR)))
	{
		return FALSE;
	}

	memcpy(nameCopy, process->Buffer, process->Length);

	for (i = 0; i < BadProcessnameCount; i++)
	{
		if (!_wcsnicmp(nameCopy, BadProcessnameList[i], process->Length))
		{
			return TRUE;
		}
	}

	return FALSE;
}

