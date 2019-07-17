#pragma once
#include "public.h"
#include "define.h"
#include "VTasm32.h"
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

//VMCS init��ʼ������
//����Vmcs ��ʼ��
NTSTATUS NTAPI ControlVmcsInit();


NTSTATUS NTAPI HostVmcsInit();

NTSTATUS NTAPI GuestVmcsInit();

// ��ȡ����������ַ,�û���дHost���� Guest��Vmcs��Ϣ�Ķβ��ɼ�����.
ULONG
GetSegmentDescriptorBase(
    ULONG gdt_base,
    USHORT seg_selector);

//Host����ں���.
extern  void __stdcall  HostVmEntry(VOID);