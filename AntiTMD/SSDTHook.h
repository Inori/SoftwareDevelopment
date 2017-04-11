#ifndef SSDT_HOOK_H
#define SSDT_HOOK_H

typedef struct _SYSTEM_SERVICE_TABLE{
	PVOID  		ServiceTableBase;
	PVOID  		ServiceCounterTableBase;
	ULONGLONG  	NumberOfServices;
	PVOID  		ParamTableBase;
} SYSTEM_SERVICE_TABLE, *PSYSTEM_SERVICE_TABLE;

typedef struct _SERVICE_DESCRIPTOR_TABLE{
	SYSTEM_SERVICE_TABLE ntoskrnl;  // ntoskrnl.exe (native api)
	SYSTEM_SERVICE_TABLE win32k;    // win32k.sys   (gdi/user)
	SYSTEM_SERVICE_TABLE Table3;    // not used
	SYSTEM_SERVICE_TABLE Table4;    // not used
}SERVICE_DESCRIPTOR_TABLE, *PSERVICE_DESCRIPTOR_TABLE;



extern PSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;


BOOLEAN HookSSDT(ULONG index, PVOID ProxyFunc, PVOID* OldFunc);
BOOLEAN UnHookSSDT(ULONG index, PVOID OldFunc);


#endif