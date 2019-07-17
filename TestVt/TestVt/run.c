#include "Driver.h"
#include "VT32.h"

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
    KdBreakPoint();
    EFLAGS eflags;
    NTSTATUS ntStatus;
    if (!NT_SUCCESS(IsSupportedVmx()))
    {
        return STATUS_UNSUCCESSFUL;
    }
    ntStatus = TurnOnTheCr4(TRUE);
    if (!NT_SUCCESS(ntStatus))
    {
        return STATUS_UNSUCCESSFUL;
    }
   //���֧��VT,����VT
    KdBreakPoint();
    if (!NT_SUCCESS(TrunOnVmxOn()))
    {
        //ʧ��ֱ���ͷ�
        TrunOffVmxOff();
    }
    KdBreakPoint();
    VmcsSupport();//����VMCS֧��
    SetVcms();//����VMCS
    Asm_VmLaunch(); //���������
    *(PULONG)&eflags = Asm_GetEflags();

    if (eflags.CF == 1 || eflags.ZF == 1)
    {
        KdBreakPoint();
        KdPrint(("���������ʧ��,ִ��vmLanuchʧ��,������� [%p] \r\n", Asm_VmRead(VM_INSTRUCTION_ERROR)));
        TrunOffVmxOff();
        return STATUS_UNSUCCESSFUL;
    }
    //���ʧ��.��ȡ������.
    
    TrunOffVmxOff();
    
    return TRUE;
}