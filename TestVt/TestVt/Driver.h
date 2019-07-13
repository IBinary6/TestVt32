#pragma once

#include "public.h"
#include <ntstrsafe.h>

#define SYMBOLIC_LINK_NAME L"\\DosDevices\\InterceptLink"
#define DEVICE_NAME        L"\\device\\InterceptDevice"

#define IOCTL_BASE          0x800
#define MY_CTL_CODE(i)        \
    CTL_CODE                  \
    (                         \
        FILE_DEVICE_UNKNOWN,  \
        IOCTL_BASE + i,       \
        METHOD_BUFFERED,      \
        FILE_ANY_ACCESS       \
    )

#define IO_START_VT            MY_CTL_CODE(0)


DRIVER_UNLOAD DriverUnLoad;
NTSTATUS InitDeviceAnSybolicLinkName(PDRIVER_OBJECT pDriverObj);
NTSTATUS InitDisPatchFunction(PDRIVER_OBJECT pDriverObj);

NTSTATUS DisPatchControl(PDEVICE_OBJECT pDriverObj, PIRP pIrp);
NTSTATUS DisPatchCommand(PDEVICE_OBJECT pDriverObj, PIRP pIrp);
NTSTATUS DisPatchRead(PDEVICE_OBJECT pDriverObj, PIRP pIrp);
NTSTATUS DisPatchWrite(PDEVICE_OBJECT pDriverObj, PIRP pIrp);
NTSTATUS DisPatchClose(PDEVICE_OBJECT pDriverObj, PIRP pIrp);
NTSTATUS DisPatchCreate(PDEVICE_OBJECT pDriverObj, PIRP pIrp);
NTSTATUS InitVt();
