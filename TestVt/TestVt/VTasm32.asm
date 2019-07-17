
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

Asm_GetCr0 proc
    mov eax,cr0
    ret
Asm_GetCr0 endp

Asm_GetCr3 proc
    mov eax,cr3
    ret
Asm_GetCr3 endp
;CS SS DS ES FS GS TR
Asm_GetCs PROC
    xor eax,eax
    mov ax,cs
    ret
Asm_GetCs endp

Asm_GetSs PROC
    xor eax,eax
    mov ax,ss
    ret
Asm_GetSs endp

Asm_GetDs proc
    xor eax,eax
    mov ax,ds
    ret
Asm_GetDs endp

Asm_GetEs proc
    xor eax,eax
    mov ax,es
    ret
Asm_GetEs endp
; fs gs tr

Asm_GetFs proc
    xor eax,eax
    mov ax,fs
    ret
Asm_GetFs endp

Asm_GetGs proc
    xor eax,eax
    mov ax,gs
    ret
Asm_GetGs endp

Asm_GetTr proc
    xor eax,eax
    str ax
    ret
Asm_GetTr endp

Asm_GetLdtr proc
    xor eax,eax
    sldt ax
    ret
Asm_GetLdtr endp

Asm_GetGdtrBase proc
    local gdtr[6]:byte
    sgdt  gdtr
    mov eax,dword ptr gdtr[2]
    ret
Asm_GetGdtrBase endp
 
Asm_GetIdtBase proc
    local idtr[6]:byte
    sidt  idtr
    mov eax,dword ptr idtr[2]
    ret
Asm_GetIdtBase endp
;Host入口

HostVmEntry proc
  int 3
  int 3
  int 3
  int 3
  int 3
  mov eax,eax
  mov ebx,ebx
  mov ecx,ecx
  mov edx,edx
  ret
HostVmEntry endp
end