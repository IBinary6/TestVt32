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