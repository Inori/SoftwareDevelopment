#include "Tools.h"

void* GetPEBLocation(HANDLE hProcess)
{
	ULONG RequiredLen = 0;
	void * PebAddress = 0;
	PROCESS_BASIC_INFORMATION myProcessBasicInformation[5] = { 0 };

	if (NtQueryInformationProcess(hProcess, ProcessBasicInformation, myProcessBasicInformation, sizeof(PROCESS_BASIC_INFORMATION), &RequiredLen) == STATUS_SUCCESS)
	{
		PebAddress = (void*)myProcessBasicInformation->PebBaseAddress;
	}
	else
	{
		if (NtQueryInformationProcess(hProcess, ProcessBasicInformation, myProcessBasicInformation, RequiredLen, &RequiredLen) == STATUS_SUCCESS)
		{
			PebAddress = (void*)myProcessBasicInformation->PebBaseAddress;
		}
	}

	return PebAddress;
}

void SetRemotePebBeingDebuged(DWORD nPid, BYTE nBeingDebuged)
{
	PEB32 stPEB = { 0 };
	HANDLE hProcess = NULL;
	void * pPEBAddr = NULL;

	hProcess= OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, 0, nPid);
	if (!hProcess)
	{
		return;
	}

	pPEBAddr = GetPEBLocation(hProcess);
	ReadProcessMemory(hProcess, pPEBAddr, (void*)&stPEB, 0x10, 0);
	stPEB.BeingDebugged = nBeingDebuged;
	WriteProcessMemory(hProcess, pPEBAddr, (void*)&stPEB, 0x10, 0);
}

