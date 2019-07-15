#pragma once
#include "public.h"
#include "define.h"
// 检测是否支持VT
NTSTATUS NTAPI IsSupportedVmx(); 
//设置CR4开启VT
NTSTATUS NTAPI TurnOnTheCr4(BOOLEAN isOpen);

//开启VMXON进入 VMX模式
NTSTATUS NTAPI TrunOnVmxOn();

//结束VMX模式.
NTSTATUS NTAPI TrunOffVmxOff();

//添加VMCS支持.
NTSTATUS NTAPI VmcsSupport();
//设置vmcs值.
NTSTATUS NTAPI SetVcms();

//vmcs中设置控制寄存器的时候设置为不处理.
static ULONG SetControlBitValue(ULONG Ctl, ULONG Msr);