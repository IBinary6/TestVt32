#pragma once
#include "public.h"
#include "define.h"
// ����Ƿ�֧��VT
NTSTATUS NTAPI IsSupportedVmx(); 
//����CR4����VT
NTSTATUS NTAPI TurnOnTheCr4(BOOLEAN isOpen);

//����VMXON���� VMXģʽ
NTSTATUS NTAPI TrunOnVmxOn();

//����VMXģʽ.
NTSTATUS NTAPI TrunOffVmxOff();

//���VMCS֧��.
NTSTATUS NTAPI VmcsSupport();
//����vmcsֵ.
NTSTATUS NTAPI SetVcms();

//vmcs�����ÿ��ƼĴ�����ʱ������Ϊ������.
static ULONG SetControlBitValue(ULONG Ctl, ULONG Msr);