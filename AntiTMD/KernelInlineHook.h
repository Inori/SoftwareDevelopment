#ifndef KERNEL_INLINE_HOOK_H
#define KERNEL_INLINE_HOOK_H


#define kmalloc(_s) ExAllocatePoolWithTag(NonPagedPool, _s, 'SYSQ')
#define kfree(_p) ExFreePool(_p)


//Hook³õÊ¼»¯º¯Êý


void DisableWriteProtect();
void EnableWriteProtect();

void *FindKernelFunction(PCWSTR FunctionName);

VOID KernelInlineHookInit();
VOID KernelInlineHookDrop();

VOID KernelInlineHook(IN PVOID ApiAddress, IN PVOID Proxy_ApiAddress, OUT PVOID *Original_ApiAddress, OUT ULONG *PatchSize);
VOID KernelInlineUnHook(IN PVOID ApiAddress, IN PVOID OriCode, IN ULONG PatchSize);

#endif