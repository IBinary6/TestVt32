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

//获取标志寄存器
ULONG __stdcall  Asm_GetEflags();

//读取CR0寄存器
ULONG __stdcall Asm_ReadCr0();


//清空vmcs
VOID __stdcall Asm_VmClear(ULONG LowPtr, ULONG HightPtr);
//装载vmcs,变为激活状态.
VOID __stdcall Asm_VmPtrld(ULONG LowPtr, ULONG HightPtr);




//读取
ULONG __stdcall Asm_VmRead(ULONG Filead);
//写入
VOID __stdcall  Asm_VmWrite(ULONG Field, ULONG Value);
//进入虚拟机
VOID __stdcall Asm_VmLaunch();


