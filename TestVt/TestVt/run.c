#include "Driver.h"
#include "VT32.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegPath)
{
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
    if (!NT_SUCCESS(IsSupportedVmx()))
    {
        return STATUS_UNSUCCESSFUL;
    }
   //如果支持VT,开启VT
    if (!NT_SUCCESS(TurnOnTheCr4(TRUE)))
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (!NT_SUCCESS(TrunOnVmxOn()))
    {
        //失败直接释放
        TrunOffVmxOff();
    }

    TrunOffVmxOff();
    return TRUE;
}