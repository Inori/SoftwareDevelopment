/*********************************************************************
Author:     Dale Roberts - Henrik Haftmann
Date:       8/30/95 - 150420
Program:    GIVEIO.SYS
Compile:    Use DDK BUILD facility
Purpose:    Give direct port I/O access to a user mode process. Win64
*********************************************************************/
#include <ntddk.h>

//amd64.asm exports:
// void Ke386SetIoAccessMap(int, IOPM*);
// void Ke386QueryIoAccessMap(int, IOPM*);
// void Ke386IoSetAccessProcess(PEPROCESS, int);
void SetIOPermissionMap(int);
void EachProcessorDpc(KDPC*,PVOID,PVOID,PVOID);

// IRQL == DISPATCH_LEVEL! (Prozessor darf nicht wechseln.)
// Ruft die angegebene, in Assembler geschriebene Callback-Routine
// für alle Prozessoren auf mit ECX = Argument.
// Blockiert (per Eigenbau-Spinlock) den aktuellen Prozessor (Thread)
// bis alle Prozessoren den Kode ausgeführt haben.
static void EachProcessor(void(*Callback)(void*),void* Arg) {
 ULONG i;
 volatile KAFFINITY a;
 a=KeQueryActiveProcessors();
 if (a==1) Callback(Arg);			// Einzelprozessorsystem (Abk.)
 else{
  KAFFINITY m;
  KDPC *Dpc,*pDpc;
  Dpc=ExAllocatePoolWithTag(NonPagedPool,sizeof(KDPC)*MAXIMUM_PROCESSORS,'tplE');
  if (!Dpc) return;
  for (i=0,m=1,pDpc=Dpc; a>=m && i<MAXIMUM_PROCESSORS; i++,m<<=1,pDpc++) if (a&m) {
// Für den aktiven Prozessor direkt aufrufen, sonst unnötiges Kuddelmuddel mit IRQL
   if (i==KeGetCurrentProcessorNumber()) EachProcessorDpc(NULL,Callback,Arg,(void*)&a);
   else{
    KeInitializeDpc(pDpc,EachProcessorDpc,Callback);
    KeSetTargetProcessorDpc(pDpc,(char)i);
    KeInsertQueueDpc(pDpc,Arg,(void*)&a);
   }
  }
  while (a);	// warten bis alle fertig sind! (Aua! Aber besser geht's wohl nicht.)
  ExFreePoolWithTag(Dpc,'tplE');
 }
}

/*********************************************************************
 Set the global IOPM (I/O permission map) so that it is given full I/O access.
 If OnFlag is 1, the process is given I/O access.
 If it is 0, access is removed.
 IRQL is DISPATCH_LEVEL while in startup or shutdown routine!
*********************************************************************/
static void GiveIO(int OnFlag) {
// KIRQL irql=KeRaiseIrqlToDpcLevel();
// __debugbreak();
 EachProcessor((void(*)(void*))SetIOPermissionMap,(void*)(__int64)OnFlag);
// KeLowerIrql(irql);
}

/*********************************************************************
  Release any allocated objects. Will be called on "sc stop giveio".
*********************************************************************/
static void GiveioUnload(const DRIVER_OBJECT*DO) {
 UNICODE_STRING us;
 RtlInitUnicodeString(&us,L"\\??\\giveio");
 IoDeleteSymbolicLink(&us);
 RtlInitUnicodeString(&us,L"\\??\\dlportio");
 IoDeleteSymbolicLink(&us);
 GiveIO(FALSE);
 IoDeleteDevice(DO->DeviceObject);
}

/*********************************************************************
  Service handler for a CreateFile() user mode call.
  Does nothing than succeed. For compatibility to 32 bit drivers only.
*********************************************************************/
static NTSTATUS OnCreate(PDEVICE_OBJECT o, PIRP I) {
// __debugbreak();
 GiveIO(TRUE);	// Re-enable I/O passthrough, in case of loss (150625)
 I->IoStatus.Information = 0;
 I->IoStatus.Status = STATUS_SUCCESS;
 IoCompleteRequest(I,IO_NO_INCREMENT);
 return STATUS_SUCCESS;
}

// Sorry, I couldn't get _any_ OnPower notification when pressing
// the Sleep button on my desktop computer.
// Furthermore, processor state was still preserved,
// no need for GiveIO(TRUE). heha, 150626
static NTSTATUS OnPower(PDEVICE_OBJECT o, PIRP I) {
 IO_STACK_LOCATION*irpStack;
 PoStartNextPowerIrp(I);
 irpStack=IoGetCurrentIrpStackLocation(I);
// __debugbreak();
 if (irpStack->MinorFunction==IRP_MN_SET_POWER
 &&  irpStack->Parameters.Power.Type==SystemPowerState
 &&  irpStack->Parameters.Power.State.SystemState==PowerDeviceD0) {
  GiveIO(TRUE);	// Re-enable I/O passthrough after hibernate or standby
 }
 return STATUS_SUCCESS;
}

/*********************************************************************
 Driver Entry routine.

 This routine is called only once after the driver is initially
 loaded into memory. (after "sc start giveio")
 It creates a symbolic link to the device driver.
 This allows a user mode application to access our driver using 
 \\.\giveio or \\.\dlportio name.
*********************************************************************/
NTSTATUS DriverEntry(PDRIVER_OBJECT DO, PUNICODE_STRING regpath) {

 PDEVICE_OBJECT dev;
 NTSTATUS status;
 UNICODE_STRING uniNameString, uniDOSString;
	//  Set up device driver name and device object.
 RtlInitUnicodeString(&uniNameString,L"\\Device\\giveio");
 status = IoCreateDevice(DO,0,&uniNameString,FILE_DEVICE_UNKNOWN,0,FALSE,&dev);
 if (!NT_SUCCESS(status)) return status;

 GiveIO(TRUE);			// give all processes I/O access

 RtlInitUnicodeString(&uniDOSString,L"\\??\\giveio");
 status = IoCreateSymbolicLink (&uniDOSString,&uniNameString);
 if (!NT_SUCCESS(status)) return status;

 RtlInitUnicodeString(&uniDOSString,L"\\??\\dlportio");
 status = IoCreateSymbolicLink (&uniDOSString,&uniNameString);
 if (!NT_SUCCESS(status)) return status;
// Initialize the Driver Object with driver's entry points.
// We require the Create and Unload operations.
 DO->MajorFunction[IRP_MJ_CREATE]=OnCreate;
 DO->DriverUnload = GiveioUnload;
// For surviving sleep mode, Power notifications must be catched. (?)
 DO->MajorFunction[IRP_MJ_POWER]=OnPower;
 return STATUS_SUCCESS;
}
