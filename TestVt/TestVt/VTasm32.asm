
;VT技术需要的汇编
.686p
.model flat, stdcall
option casemap:none

.code

Asm_CheckExecuteCpuId32	Proc uses ebx esi edi fn:dword, ret_eax:dword,ret_ebx:dword,ret_ecx:dword, ret_edx:dword
	
	
        mov	eax, fn             ;传入fn进行执行cpuid,结果放在 eax ebx ecx edx中
        cpuid
        mov	esi, ret_eax        ;获取eax值 .eax是一个地址.
        mov	dword ptr [esi], eax;将返回值eax返回到 eax指向的地址中.下面一样.
        mov	esi, ret_ebx
        mov	dword ptr [esi], ebx
        mov	esi, ret_ecx
        mov	dword ptr [esi], ecx
        mov	esi, ret_edx
        mov	dword ptr [esi], edx
        ret
Asm_CheckExecuteCpuId32 	endp

;读取MSR寄存器. MSR在inter以后都成为MSR了.

Asm_ReadMsr32 proc ReadIndex:dword
         mov ecx,ReadIndex
         rdmsr
         ret
Asm_ReadMsr32 endp

Asm_GetCr432 proc 
     mov eax,cr4
     ret
Asm_GetCr432 endp

Asm_SetCr432 proc NewValue:dword
  mov eax,NewValue
  mov cr4,eax
  ret
Asm_SetCr432 endp

Asm_VmxOn proc LowPtr:dword , HightPtr:dword
    push HightPtr
    push LowPtr
    vmxon	qword ptr [esp]
    add esp,8
    ret
Asm_VmxOn endp

Asm_VmxOff proc
    vmxoff
    ret
Asm_VmxOff endp


Asm_GetEflags proc
    pushfd
    pop eax
    ret
Asm_GetEflags endp

Asm_ReadCr0 proc
    mov eax,cr0
    ret
Asm_ReadCr0 endp


Asm_VmLaunch proc 
    vmlaunch
    ret
Asm_VmLaunch endp


Asm_VmRead proc uses ecx Filead:dword   
     mov eax,Filead
     vmread ecx,eax
     mov eax,ecx
     ret
Asm_VmRead endp

Asm_VmWrite proc uses ecx Field:dword,Value:dword
    mov eax,Field
    mov ecx,Value
    vmwrite eax,ecx
    ret
Asm_VmWrite endp


Asm_VmClear proc LowPtr:dword, HightPtr:dword
    push HightPtr
    push LowPtr
    vmclear qword ptr [esp]
    add esp,8
    ret
Asm_VmClear endp

Asm_VmPtrld proc LowPtr:dword,HightPtr:dword
    push HightPtr
    push LowPtr
    vmptrld qword ptr[esp]
    add esp,8
    ret
Asm_VmPtrld endp


end