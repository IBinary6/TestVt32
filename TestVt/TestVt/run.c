#include "Driver.h"
#include "VT32.h"
#include "VTasm32.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegPath)
{
    pDriverObj->DriverUnload = DriverUnLoad;
	/*ULONG iCount = 0;
	NTSTATUS ntStatus;
	pDriverObj->DriverUnload = DriverUnLoad;
	ntStatus = InitDeviceAnSybolicLinkName(pDriverObj);
	if (!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}

	ntStatus = InitDisPatchFunction(pDriverObj);
	if (!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}*/
    KdPrint(("VT����������\r\n"));
    InitVt();
  
	return STATUS_SUCCESS;
}

NTSTATUS InitVt()
{
    EFLAGS eflags;
    if (!NT_SUCCESS(IsSupportedVmx()))
    {
        return STATUS_UNSUCCESSFUL;
    }
   //���֧��VT,����VT
    if (!NT_SUCCESS(TurnOnTheCr4(TRUE)))
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (!NT_SUCCESS(TrunOnVmxOn()))
    {
        //ʧ��ֱ���ͷ�
        TrunOffVmxOff();
    }
    
    VmcsSupport();//����VMCS֧��
    
    SetVcms();//����VMCS
    Asm_VmLaunch(); //���������
    *(PULONG)&eflags = Asm_GetEflags();

    if (eflags.CF == 1 || eflags.ZF == 1)
    {
        KdPrint(("���������ʧ��,ִ��vmLanuchʧ��,������� = %d \r\n", Asm_VmRead(VM_INSTRUCTION_ERROR)));
        TrunOffVmxOff();
        return STATUS_UNSUCCESSFUL;
    }
    //���ʧ��.��ȡ������.
    
    TrunOffVmxOff();
    
    return TRUE;
}