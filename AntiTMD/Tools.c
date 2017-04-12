#include <ntddk.h>
#include "Tools.h"


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



BOOLEAN InitUndocFunctions()
{
	if (NULL == ZwQueryInformationProcess)
	{
		UNICODE_STRING usRoutineName;
		RtlInitUnicodeString(&usRoutineName, L"ZwQueryInformationProcess");
		ZwQueryInformationProcess =
			(pfuncZwQueryInformationProcess)MmGetSystemRoutineAddress(&usRoutineName);
		if (NULL == ZwQueryInformationProcess)
		{
			return FALSE;
		}
	}
}
