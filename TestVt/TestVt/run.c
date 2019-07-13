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
    KdPrint(("VT����������\r\n"));
    InitVt();
  
	return STATUS_SUCCESS;
}

NTSTATUS InitVt()
{
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

    TrunOffVmxOff();
    return TRUE;
}