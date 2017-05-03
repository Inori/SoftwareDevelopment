#pragma once

#include "ntdll.h"

typedef struct _PEB32
{
	union
	{
		struct
		{
			BYTE InheritedAddressSpace;
			BYTE ReadImageFileExecOptions;
			BYTE BeingDebugged;
			BYTE _SYSTEM_DEPENDENT_01;
		};
		DWORD dummy01;
	};
	DWORD Mutant;
	DWORD ImageBaseAddress;
	DWORD Ldr;
	DWORD ProcessParameters;
	DWORD SubSystemData;
	DWORD ProcessHeap;
	DWORD FastPebLock;
	DWORD _SYSTEM_DEPENDENT_02;
	DWORD _SYSTEM_DEPENDENT_03;
	DWORD _SYSTEM_DEPENDENT_04;
	union
	{
		DWORD KernelCallbackTable;
		DWORD UserSharedInfoPtr;
	};
	DWORD SystemReserved;
	DWORD _SYSTEM_DEPENDENT_05;
	DWORD _SYSTEM_DEPENDENT_06;
	DWORD TlsExpansionCounter;
	DWORD TlsBitmap;
	DWORD TlsBitmapBits[2];
	DWORD ReadOnlySharedMemoryBase;
	DWORD _SYSTEM_DEPENDENT_07;
	DWORD ReadOnlyStaticServerData;
	DWORD AnsiCodePageData;
	DWORD OemCodePageData;
	DWORD UnicodeCaseTableData;
	DWORD NumberOfProcessors;
	union
	{
		DWORD NtGlobalFlag;
		DWORD64 dummy02;
	};
	LARGE_INTEGER CriticalSectionTimeout;
	DWORD HeapSegmentReserve;
	DWORD HeapSegmentCommit;
	DWORD HeapDeCommitTotalFreeThreshold;
	DWORD HeapDeCommitFreeBlockThreshold;
	DWORD NumberOfHeaps;
	DWORD MaximumNumberOfHeaps;
	DWORD ProcessHeaps;

	//FULL PEB not needed
	/*    DWORD GdiSharedHandleTable;
	DWORD ProcessStarterHelper;
	DWORD GdiDCAttributeList;
	DWORD LoaderLock;
	DWORD OSMajorVersion;
	DWORD OSMinorVersion;
	WORD OSBuildNumber;
	WORD OSCSDVersion;
	DWORD OSPlatformId;
	DWORD ImageSubsystem;
	DWORD ImageSubsystemMajorVersion;
	DWORD ImageSubsystemMinorVersion;
	union
	{
	DWORD ImageProcessAffinityMask;
	DWORD ActiveProcessAffinityMask;
	};
	DWORD GdiHandleBuffer[34];
	DWORD PostProcessInitRoutine;
	DWORD TlsExpansionBitmap;
	DWORD TlsExpansionBitmapBits[32];
	DWORD SessionId;
	ULARGE_INTEGER AppCompatFlags;
	ULARGE_INTEGER AppCompatFlagsUser;
	DWORD pShimData;
	DWORD AppCompatInfo;
	UNICODE_STRING_T<DWORD> CSDVersion;
	DWORD ActivationContextData;
	DWORD ProcessAssemblyStorageMap;
	DWORD SystemDefaultActivationContextData;
	DWORD SystemAssemblyStorageMap;
	DWORD MinimumStackCommit;*/
} PEB32, *PPEB32;

void SetRemotePebBeingDebuged(DWORD nPid, BYTE nBeingDebuged);