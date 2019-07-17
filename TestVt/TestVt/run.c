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
    KdPrint(("VT驱动启动中\r\n"));
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
   //如果支持VT,开启VT
    KdBreakPoint();
    if (!NT_SUCCESS(TrunOnVmxOn()))
    {
        //失败直接释放
        TrunOffVmxOff();
    }
    KdBreakPoint();
    VmcsSupport();//启动VMCS支持
    SetVcms();//设置VMCS
    Asm_VmLaunch(); //进入虚拟机
    *(PULONG)&eflags = Asm_GetEflags();

    if (eflags.CF == 1 || eflags.ZF == 1)
    {
        KdBreakPoint();
        KdPrint(("进入虚拟机失败,执行vmLanuch失败,错误代码 [%p] \r\n", Asm_VmRead(VM_INSTRUCTION_ERROR)));
        TrunOffVmxOff();
        return STATUS_UNSUCCESSFUL;
    }
    //如果失败.读取错误码.
    
    TrunOffVmxOff();
    
    return TRUE;
}