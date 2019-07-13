#include "VT32.h"
#include "VTasm32.h"

G_CPU g_CpuGlobalValueInfo; //��¼��CPUȫ�ֱ�����Ϣ
NTSTATUS NTAPI IsSupportedVmx()
{
    //����Ƿ�֧��VMXģʽ
    //PAE ģʽ�ο����� https://www.cnblogs.com/ck1020/p/6078214.html
    ULONG64 Msr;
    ULONG eax, ebx, ecx, edx;
    ULONG64 Cr4;
    CR0_FLAGS Cr0;
    Asm_CheckExecuteCpuId32(0, &eax, &ebx, &ecx, &edx);
    //ִ��cpuid֮��.0 1 ���ߴ���0x80000000 �ֱ��в�ͬ����,������Ϣ.�Լ���չ��Ϣ.
    if (eax < 1)
    {
        KdPrint(("ָ�֧��CPUID\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //���CPU����
    if (!(ebx == 0x756e6547 && ecx == 0x6c65746e && edx == 0x49656e69))
    {
        KdPrint(("����Inter����\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    *((PULONG)&Cr0) = Asm_ReadCr0();
    if (Cr0.PE != 1 && Cr0.PG != 1)
    {
        //PE = 1������ģʽ PE = 0����ʵģʽ
        //PG ��ҳ����. PG = 1.�����ַת���Ե�ַȻ��תΪ�����ַ
        KdPrint(("VT��⿪������.���뿪������ģʽ.�Լ���ҳ����\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //���VMXģʽ����λ VMX[5] ���=1����֧��VMXģʽ.����֧��.
    Asm_CheckExecuteCpuId32(1, &eax, &ebx, &ecx, &edx);
    if (!(ecx & 0x20))
    {
        KdPrint(("�Բ���,��֧��VMXģʽ\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //���BIOS �Ƿ�֧��VT
    Msr = Asm_ReadMsr32(IA32_FEATURE_CONTROL);
    if (!(Msr & 1))
    {
        KdPrint(("֧��VT����,������BIOS��û������VT\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //����CR4�ĵ�14λΪ1.���п���VMXģʽ.
    Cr4 = Asm_GetCr432();
    /*  Asm_SetCr432(Cr4 | CR4_VMX_Model);
      Cr4 = Asm_GetCr432();*/
    if ((Cr4 & CR4_VMX_Model) == 1)
    {
        KdPrint(("CR4�Ѿ�������VT.�ѱ�ռ��.��ر���������\r\n"));
        return STATUS_UNSUCCESSFUL;
    }

    KdPrint(("ϵͳ֧��VMXģʽ\r\n"));
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI TurnOnTheCr4(BOOLEAN isOpen)
{
    //��CR4�ĵ�14λ CR4[13] = 1���ǿ���VT
  
   CR4_FLAGS Cr4;
    *(PULONG)&Cr4 = Asm_GetCr432();
    if (TRUE == isOpen)
    {
        if (Cr4.VMXE == 1)
        {
            KdPrint(("VT�Ѿ���������,CR4ֵΪ1\r\n"));
            return STATUS_SUCCESS;
        }
        else
        {
            Cr4.VMXE = 1; 
            Asm_SetCr432(*(PULONG)&Cr4);
            KdPrint(("��������VTCR4������,��ȴ�......\r\n"));
        }
        if (!(Cr4.VMXE))
        {
            KdPrint(("����VTģʽʧ��,����CR4����\r\n"));
            return STATUS_UNSUCCESSFUL;
        }
        KdPrint(("����CR4�ɹ�,����VTģʽ\r\n"));
    }
    else
    {
        if (Cr4.VMXE != 1)
        {
           
            return STATUS_UNSUCCESSFUL;
        }
        else
        {
            Cr4.VMXE = 0;
            Asm_SetCr432(*(PULONG)&Cr4);
            return STATUS_SUCCESS;
        }

    }
    
   
    return STATUS_SUCCESS;
}


NTSTATUS NTAPI TrunOnVmxOn()
{
    EFLAGS eflags;
    g_CpuGlobalValueInfo.VmxOnRegion = NULL;
   // RtlZeroMemory(((void *)(&g_CpuGlobalValueInfo.VmOnxRegionPA)), sizeof(PHYSICAL_ADDRESS));

    g_CpuGlobalValueInfo.VmxOnRegion = ExAllocatePoolWithTag(NonPagedPool, 0x1000, 'XMV');
    if (NULL == g_CpuGlobalValueInfo.VmxOnRegion)
    {
        KdPrint(("��VMXONʧ��\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //Ҫ���ð汾��.���������. 00001000 0000 0001 ���4���ֽ�����Ϊ1
    RtlZeroMemory(g_CpuGlobalValueInfo.VmxOnRegion, 0x1000);
    *(PULONG)g_CpuGlobalValueInfo.VmxOnRegion = 1;
    //��Ҫ���������ַ.���Դ��������ַ.
    g_CpuGlobalValueInfo.VmOnxRegionPA = MmGetPhysicalAddress(g_CpuGlobalValueInfo.VmxOnRegion);

    //ʹ��VMXon �����д�.
    Asm_VmxOn(g_CpuGlobalValueInfo.VmOnxRegionPA.LowPart, g_CpuGlobalValueInfo.VmOnxRegionPA.HighPart);

    //���EFLAGSλ��.
    *((PULONG)&eflags) = Asm_GetEflags();
    if (eflags.CF != 0)
    {
        KdPrint(("����VMX_ONʧ��,��⵽CFλ������0 \r\n"));
        ExFreePool(g_CpuGlobalValueInfo.VmxOnRegion);
        return STATUS_UNSUCCESSFUL;
    }

    KdPrint(("����VMXON�ɹ�,����VMX hostģʽ\r\n"));
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI TrunOffVmxOff()
{
    
    KdPrint(("�ر�VTģʽ��,��鿴���\r\n"));
    Asm_VmxOff();
    if (NULL != g_CpuGlobalValueInfo.VmxOnRegion)
    {
        ExFreePool(g_CpuGlobalValueInfo.VmxOnRegion);
        KdPrint(("�ͷ�VmxOn�ڴ�ɹ�\r\n"));
    }

    //�ر�CR4�Ĵ���.
    if (!NT_SUCCESS(TurnOnTheCr4(FALSE)))
    {
        KdPrint(("VT Cr4Ϊ1 �޷�����,�ر�ʧ��\r\n"));
        return STATUS_UNSUCCESSFUL;
    }

   

    KdPrint(("�ر�VT�ɹ�,�ѳɹ��ر�\r\n"));
    return STATUS_SUCCESS;
}