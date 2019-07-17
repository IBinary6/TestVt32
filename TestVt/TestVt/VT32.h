#pragma once
#include "public.h"
#include "define.h"
#include "VTasm32.h"
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

//VMCS init初始化函数
//控制Vmcs 初始化
NTSTATUS NTAPI ControlVmcsInit();


NTSTATUS NTAPI HostVmcsInit();

NTSTATUS NTAPI GuestVmcsInit();

// 获取段描述符基址,用户填写Host或者 Guest的Vmcs信息的段不可见部分.
ULONG
GetSegmentDescriptorBase(
    ULONG gdt_base,
    USHORT seg_selector);

//Host的入口函数.
extern  void __stdcall  HostVmEntry(VOID);