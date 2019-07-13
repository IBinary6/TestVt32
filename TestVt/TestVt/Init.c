
#include "Driver.h"



NTSTATUS InitDisPatchFunction(PDRIVER_OBJECT pDriverObj)
{
	ULONG iCount = 0;
	for (iCount = 0; iCount < IRP_MJ_MAXIMUM_FUNCTION; iCount++)
	{
		pDriverObj->MajorFunction[iCount] = DisPatchCommand;
	}

	pDriverObj->MajorFunction[IRP_MJ_CREATE] = DisPatchCreate;
	pDriverObj->MajorFunction[IRP_MJ_READ] = DisPatchRead;
	pDriverObj->MajorFunction[IRP_MJ_WRITE] = DisPatchWrite;
	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DisPatchControl;
	pDriverObj->MajorFunction[IRP_MJ_CLOSE] = DisPatchClose;

	return STATUS_SUCCESS;

}
NTSTATUS InitDeviceAnSybolicLinkName(PDRIVER_OBJECT pDriverObj)
{
	UNICODE_STRING uDeviceName;
	UNICODE_STRING uSymbolicLinkName;
	PDEVICE_OBJECT pDeviceObj;
	NTSTATUS ntStatus;

	RtlUnicodeStringInit(&uDeviceName, DEVICE_NAME);
	RtlUnicodeStringInit(&uSymbolicLinkName, SYMBOLIC_LINK_NAME);
	//创建设备对象
	
	ntStatus = IoCreateDevice(
		pDriverObj,
		0,
		&uDeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&pDeviceObj
	);
	if (!NT_SUCCESS(ntStatus))
	{
		KdPrint(("创建设备对象失败"));
		return ntStatus;
	}
	pDeviceObj->Flags = DO_BUFFERED_IO;

	ntStatus = IoCreateSymbolicLink(&uSymbolicLinkName, &uDeviceName);
	//创建符号链接
	if (!NT_SUCCESS(ntStatus))
	{
		IoDeleteDevice(pDeviceObj);
		KdPrint(("创建符号链接失败"));
		return ntStatus;
	}
	return STATUS_SUCCESS;
}

NTSTATUS DisPatchControl(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{

	PVOID pUserBuffer = NULL;
	ULONG uControlCode = 0;
	ULONG pUserBufferLength = 0;
    NTSTATUS ntStatus;
	PIO_STACK_LOCATION pIrpStack;

	pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

	pUserBuffer = pIrp->AssociatedIrp.SystemBuffer;
	uControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
	pUserBufferLength = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength; //默认是写数据.
	
    switch (uControlCode)
    {
    case IO_START_VT:  //检测VT
    {
        

        ntStatus = STATUS_SUCCESS;
    }
    default:
        ntStatus = STATUS_SUCCESS;
        break;
    }

	pIrp->IoStatus.Status = ntStatus; //IRP记录这次操作与否的.
	pIrp->IoStatus.Information = pUserBufferLength;         //Information用来记录实际传输的字节数的.

	//提交请求.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //上面的 
}

NTSTATUS DisPatchRead(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{
	//获取Ring3缓冲区.因为是缓冲区读取.直接获取缓冲区.
	PVOID pUserReadBuffer = NULL;
	ULONG uPuserReadBufferLeng = 0;
	ULONG uHelloWorldLength = 0;
	ULONG uMinLength = 0;

	PIO_STACK_LOCATION pIrpStack;
	

	pUserReadBuffer = pIrp->AssociatedIrp.SystemBuffer;

	pIrpStack = IoGetCurrentIrpStackLocation(pIrp); //用的的操作在堆栈里面
	uPuserReadBufferLeng = pIrpStack->Parameters.Read.Length;
	uHelloWorldLength =(ULONG)wcslen(L"HelloWorld");

	uMinLength = uHelloWorldLength > uPuserReadBufferLeng
		? uHelloWorldLength : uPuserReadBufferLeng;

	//拷贝数据给Ring3
	__try
	{
		ProbeForRead(pUserReadBuffer, uPuserReadBufferLeng, 0);
		RtlCopyMemory(pUserReadBuffer, L"HelloWorld", uMinLength);
	}
	__except (STATUS_ACCESS_VIOLATION)
	{
		KdPrint(("出现异常了"));
	}
	
	
	

	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP记录这次操作与否的.
	pIrp->IoStatus.Information = uMinLength;         //Information用来记录实际传输的字节数的.

	//提交请求.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //上面的 
}
NTSTATUS DisPatchWrite(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{
	PVOID pUserWriteBuffer = NULL;
	ULONG uPuserWriteBufferLeng = 0;
	PVOID pKernelBuffer = NULL;

	PIO_STACK_LOCATION pIrpStack;

	pUserWriteBuffer = pIrp->AssociatedIrp.SystemBuffer;

	pIrpStack = IoGetCurrentIrpStackLocation(pIrp); //用的的操作在堆栈里面
	uPuserWriteBufferLeng = pIrpStack->Parameters.Write.Length;
	
	
	pKernelBuffer = ExAllocatePoolWithTag(PagedPool, uPuserWriteBufferLeng, 0);
	if (NULL == pKernelBuffer)
	{
		KdPrint(("对不起.写内存错误"));
		pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP记录这次操作与否的.
		pIrp->IoStatus.Information = 0;         //Information用来记录实际传输的字节数的.

		//提交请求.
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
	RtlCopyMemory(pKernelBuffer, pUserWriteBuffer, uPuserWriteBufferLeng);
	
	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP记录这次操作与否的.
	pIrp->IoStatus.Information = uPuserWriteBufferLeng;         //Information用来记录实际传输的字节数的.

	//提交请求.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //上面的 
}

NTSTATUS DisPatchCommand(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP记录这次操作与否的.
	pIrp->IoStatus.Information = 0;         //Information用来记录实际传输的字节数的.

	//提交请求.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //上面的 
}

NTSTATUS DisPatchClose(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP记录这次操作与否的.
	pIrp->IoStatus.Information = 0;         //Information用来记录实际传输的字节数的.

	//提交请求.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //上面的 
}
NTSTATUS DisPatchCreate(PDEVICE_OBJECT pDriverObj, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS; //IRP记录这次操作与否的.
	pIrp->IoStatus.Information = 0;         //Information用来记录实际传输的字节数的.

	//提交请求.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;                  //上面的 
}
void DriverUnLoad(PDRIVER_OBJECT pDriverObj)
{
	//释放资源.删除设备对象.

    /*UNICODE_STRING uSymbolicLinkName;

    RtlUnicodeStringInit(&uSymbolicLinkName, SYMBOLIC_LINK_NAME);

    IoDeleteDevice(pDriverObj->DeviceObject);
    IoDeleteSymbolicLink(&uSymbolicLinkName);*/
	KdPrint(("驱动卸载成功"));
}
