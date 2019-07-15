#pragma once
#include "public.h"

VOID __stdcall Asm_CheckExecuteCpuId32(ULONG fn,
    OUT PULONG ret_eax,
    OUT PULONG ret_ebx,
    OUT PULONG ret_ecx,
    OUT PULONG ret_edx);

ULONG64 __stdcall Asm_ReadMsr32(ULONG ReadIndex);

ULONG __stdcall Asm_GetCr432();
VOID __stdcall Asm_SetCr432(ULONG_PTR NewValue);


VOID __stdcall Asm_VmxOn(ULONG LowPtr, ULONG HightPtr);
VOID __stdcall Asm_VmxOff();

//��ȡ��־�Ĵ���
ULONG __stdcall  Asm_GetEflags();

//��ȡCR0�Ĵ���
ULONG __stdcall Asm_ReadCr0();


//���vmcs
VOID __stdcall Asm_VmClear(ULONG LowPtr, ULONG HightPtr);
//װ��vmcs,��Ϊ����״̬.
VOID __stdcall Asm_VmPtrld(ULONG LowPtr, ULONG HightPtr);




//��ȡ
ULONG __stdcall Asm_VmRead(ULONG Filead);
//д��
VOID __stdcall  Asm_VmWrite(ULONG Field, ULONG Value);
//���������
VOID __stdcall Asm_VmLaunch();


