;Anscheinend h�lt Windows (64bit) die 8 KByte IOPM in jedem TSS bereit.
;Daher gen�gt es, den Limit-Wert in der GDT entsprechend zu �ndern.

.code
extern KdDebuggerNotPresent:qword	;Pointer auf Byte

;void EachProcessorDpc(KDPC*Dpc, PVOID Context, PVOID Arg1, PVOID Arg2)
EachProcessorDpc proc
	push	r9			;Arg2 = Zeiger auf Affinit�tsmaske
	 mov	rcx,r8			;Arg1 = Argument
	 call	rdx			;Context = Prozedurzeiger
	pop	rcx
	movzx	eax,byte ptr gs:[184h]	;KeGetCurrentProcessorNumber() f�r AMD64
	lock btr dword ptr[rcx],eax	;F�r diesen Prozessor als erledigt markieren
	ret
EachProcessorDpc endp

;Zeiger in GDT f�r TSS beschaffen ->RDX
GetTssDesc proc private
	sub	rsp,16
	sgdt	[rsp+6]
	str	rdx		;Windows: 0x40
	add	rdx,[rsp+8]
	add	rsp,16
	ret
GetTssDesc endp

;GDT-Eintrag f�r aktuellen TSS auslesen, Offset->RAX, L�nge->ECX
;Typischer Aufbau: 00700067 05008B3F FFFF8000 00000000
;Anscheinend(!) reserviert Windows (Vista) bereits 8K Platz, da sind Nullen drin.
;Daher w�rde es gen�gen, einfach das Limit in der GDT (den GDTs) zu erh�hen.
;Was zu testen w�re.
GetTSS proc private
	call	GetTssDesc
	mov	eax,[rdx+8]
	shl	rax,16
	mov	ah,[rdx+7]
	mov	al,[rdx+4]
	shl	rax,16
	mov	ax,[rdx+2]
	movzx	ecx,word ptr[rdx]	;L�nge (f�r ein TSS gen�gt das)
	inc	ecx
	ret
GetTSS endp

;GDT-Eintrag f�r aktuellen TSS �ndern, Offset=RAX, L�nge=ECX
;Ruft PatchGuard auf den Plan! Dieser generiert Bluescreen 0x109 (arg4 = 3 = GDT hacked)
SetTSS proc private
	call	GetTssDesc
	pushf
	 cli
	 dec	ecx
	 mov	[rdx],cx
	 mov	[rdx+2],ax
	 shr	rax,16
	 mov	[rdx+4],al
	 mov	[rdx+7],ah
	 shr	rax,16
	 mov	[rdx+8],eax
	 str	ax
	 and	byte ptr[rdx+5],not 2	;Busy-Bit in GDT l�schen (sonst kracht's beim n�chsten Befehl)
	 ltr	ax			;Lade CPU-internen Cache des Task-Registers
	 mov	rax,qword ptr[KdDebuggerNotPresent]
	 cmp	byte ptr[rax],0
	 jz	@f			;Springe wenn Debugger vorhanden
	 mov	byte ptr[rdx+1],0	;Das geht wirklich! PatchGuard austricksen
@@:	popf
	ret
SetTSS endp

if 0
;L�dt rax=IOPM, edx=2000h, ecx=L�nge IOPM, minimal 0, maximal 8K
GetTSS2 proc private
	call	GetTSS
	movzx	rdx,word ptr[rax+102]	;I/O Map Base Address (sollte 104 =0x68 enthalten)
	add	rax,rdx			;dort f�ngt sie an
	sub	ecx,edx			;so lang ist der Rest
	jnc	@f
	xor	ecx,ecx			;0 falls negativ
@@:	xor	edx,edx
	mov	dh,20h			;8K = L�nge der IOPM
	cmp	ecx,edx
	jc	@f
	mov	ecx,edx			;auf 8K begrenzen falls zu gro�
@@:	ret
GetTSS2 endp

;void Ke386SetIoAccessMap(int, IOPM*);
Ke386SetIoAccessMap proc
	push	rsi
	 mov	rsi,rdx
	 call	GetTSS2
	 xchg	rdi,rax
	 rep	movsb			;lt. Intel-Doku wird ECX, nicht RCX zum Z�hlen verwendet!
	 xchg	rdi,rax
	pop	rsi
	ret
Ke386SetIoAccessMap endp

;void Ke386QueryIoAccessMap(int, IOPM*);
Ke386QueryIoAccessMap proc
	push	rdi
	 mov	rdi,rdx
	 call	GetTSS2
	 sub	edx,ecx			;Fehlender Teil
	 xchg	rsi,rax
	 rep	movsb			;IOPM kopieren (0 = frei, 1 = kein Zugriff)
	 xchg	rsi,rax
	 mov	ecx,edx
	 mov	al,0FFh
	 rep	stosb			;IOPM auff�llen (1 = kein Zugriff)
	pop	rdi
	ret
Ke386QueryIoAccessMap endp
endif

;void SetIOPermissionMap(int OnOff);
;Setzt TSS-Limit auf 8K oder 0 f�r den aktuellen Prozessor
SetIOPermissionMap proc
	test	ecx,ecx
	jnz	@@on
	call	GetTSS
	mov	ch,0
	jmp	SetTSS

@@on:	call	GetTSS
	mov	ch,20h
	jmp	SetTSS
SetIOPermissionMap endp

end
