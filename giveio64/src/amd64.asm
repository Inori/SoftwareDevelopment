
.code
extern KdDebuggerNotPresent:qword	

;void EachProcessorDpc(KDPC*Dpc, PVOID Context, PVOID Arg1, PVOID Arg2)
EachProcessorDpc proc
	push r9			
	mov	rcx, r8		
	call rdx	
	pop	rcx
	movzx eax,byte ptr gs:[184h]	;KeGetCurrentProcessorNumber() 
	lock btr dword ptr[rcx],eax	;Mark we are done on this processor
	ret
EachProcessorDpc endp

;Save TSS Descriptor in rdx
GetTssDesc proc private
	sub	rsp,16
	sgdt	[rsp+6]
	str	rdx		;Windows: 0x40
	shr rdx, 3  ;Skip TI and RPL bits
	shl rdx, 3  ;Multi 8 = sizeof(Segment Descriptor)
	add	rdx,[rsp+8] ;TSS Segment Descriptor (Note TSS desciptor is 16 bytes long in IA-32e mode)
	add	rsp,16
	ret
GetTssDesc endp


;Get TSS Segment in GDT
;offset -> rax, length-> ecx
;Typical item: 00700067 05008B3F FFFF8000 00000000
GetTSS proc private
	call	GetTssDesc
	mov	eax,[rdx+8]
	shl	rax,16
	mov	ah,[rdx+7]
	mov	al,[rdx+4]
	shl	rax,16
	mov	ax,[rdx+2]
	movzx ecx,word ptr[rdx]	;Length
	inc	ecx
	ret
GetTSS endp

;Intel Manual:
;The task register has a visible part (that can be read and changed by software) and an invisible part (maintained
;by the processor and is inaccessible by software).
;
;Here's a hack, as the above Intel manual said, TR has an internal cache of TSS descriptor,
;we use this cache to bypass PatchGuard
;The approach is that we first modify TSS descriptor as we need,
;then load the new TSS descriptor into cache using ltr instruction
;and finally we recover the original TSS descriptor as soon as the cache is loaded
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
	and	byte ptr[rdx+5], not 2	;Clear Busy-Bit in TSS Descriptor, or we get Bluescreen
	ltr	ax						;Load TSS descriptor into cache
	mov	rax,qword ptr[KdDebuggerNotPresent]
	cmp	byte ptr[rax],0
	jz	@f
	mov	byte ptr[rdx+1],0	;Recover original content
@@:	popf
	ret
SetTSS endp

if 0
;Lädt rax=IOPM, edx=2000h, ecx=Länge IOPM, minimal 0, maximal 8K
GetTSS2 proc private
	call	GetTSS
	movzx	rdx,word ptr[rax+102]	;I/O Map Base Address (sollte 104 =0x68 enthalten)
	add	rax,rdx			;dort fängt sie an
	sub	ecx,edx			;so lang ist der Rest
	jnc	@f
	xor	ecx,ecx			;0 falls negativ
@@:	xor	edx,edx
	mov	dh,20h			;8K = Länge der IOPM
	cmp	ecx,edx
	jc	@f
	mov	ecx,edx			;auf 8K begrenzen falls zu groß
@@:	ret
GetTSS2 endp

;void Ke386SetIoAccessMap(int, IOPM*);
Ke386SetIoAccessMap proc
	push	rsi
	 mov	rsi,rdx
	 call	GetTSS2
	 xchg	rdi,rax
	 rep	movsb			;lt. Intel-Doku wird ECX, nicht RCX zum Zählen verwendet!
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
	 rep	stosb			;IOPM auffüllen (1 = kein Zugriff)
	pop	rdi
	ret
Ke386QueryIoAccessMap endp
endif

;void SetIOPermissionMap(int OnOff);
;Set TSS-Limit to 8K or 0 on current processor
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
