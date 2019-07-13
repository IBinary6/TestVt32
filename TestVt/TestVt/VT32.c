#include "VT32.h"
#include "VTasm32.h"

G_CPU g_CpuGlobalValueInfo; //记录了CPU全局变量信息
NTSTATUS NTAPI IsSupportedVmx()
{
    //检测是否支持VMX模式
    //PAE 模式参考资料 https://www.cnblogs.com/ck1020/p/6078214.html
    ULONG64 Msr;
    ULONG eax, ebx, ecx, edx;
    ULONG64 Cr4;
    CR0_FLAGS Cr0;
    Asm_CheckExecuteCpuId32(0, &eax, &ebx, &ecx, &edx);
    //执行cpuid之后.0 1 或者大于0x80000000 分别有不同含义,基础信息.以及扩展信息.
    if (eax < 1)
    {
        KdPrint(("指令不支持CPUID\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //检测CPU厂商
    if (!(ebx == 0x756e6547 && ecx == 0x6c65746e && edx == 0x49656e69))
    {
        KdPrint(("不是Inter厂商\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    *((PULONG)&Cr0) = Asm_ReadCr0();
    if (Cr0.PE != 1 && Cr0.PG != 1)
    {
        //PE = 1代表保护模式 PE = 0代表实模式
        //PG 分页管理. PG = 1.虚拟地址转线性地址然后转为物理地址
        KdPrint(("VT检测开启条件.必须开启保护模式.以及分页管理\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //检测VMX模式第六位 VMX[5] 如果=1则是支持VMX模式.否则不支持.
    Asm_CheckExecuteCpuId32(1, &eax, &ebx, &ecx, &edx);
    if (!(ecx & 0x20))
    {
        KdPrint(("对不起,不支持VMX模式\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //检查BIOS 是否支持VT
    Msr = Asm_ReadMsr32(IA32_FEATURE_CONTROL);
    if (!(Msr & 1))
    {
        KdPrint(("支持VT技术,但是在BIOS中没有启用VT\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //设置CR4的第14位为1.进行开启VMX模式.
    Cr4 = Asm_GetCr432();
    /*  Asm_SetCr432(Cr4 | CR4_VMX_Model);
      Cr4 = Asm_GetCr432();*/
    if ((Cr4 & CR4_VMX_Model) == 1)
    {
        KdPrint(("CR4已经开启了VT.已被占用.请关闭重新设置\r\n"));
        return STATUS_UNSUCCESSFUL;
    }

    KdPrint(("系统支持VMX模式\r\n"));
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI TurnOnTheCr4(BOOLEAN isOpen)
{
    //打开CR4的第14位 CR4[13] = 1则是开启VT
  
   CR4_FLAGS Cr4;
    *(PULONG)&Cr4 = Asm_GetCr432();
    if (TRUE == isOpen)
    {
        if (Cr4.VMXE == 1)
        {
            KdPrint(("VT已经被开启过,CR4值为1\r\n"));
            return STATUS_SUCCESS;
        }
        else
        {
            Cr4.VMXE = 1; 
            Asm_SetCr432(*(PULONG)&Cr4);
            KdPrint(("正在设置VTCR4开启中,请等待......\r\n"));
        }
        if (!(Cr4.VMXE))
        {
            KdPrint(("开启VT模式失败,请检查CR4设置\r\n"));
            return STATUS_UNSUCCESSFUL;
        }
        KdPrint(("设置CR4成功,开启VT模式\r\n"));
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
        KdPrint(("打开VMXON失败\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //要设置版本号.否则会蓝屏. 00001000 0000 0001 最后4个字节设置为1
    RtlZeroMemory(g_CpuGlobalValueInfo.VmxOnRegion, 0x1000);
    *(PULONG)g_CpuGlobalValueInfo.VmxOnRegion = 1;
    //需要传入物理地址.所以传入物理地址.
    g_CpuGlobalValueInfo.VmOnxRegionPA = MmGetPhysicalAddress(g_CpuGlobalValueInfo.VmxOnRegion);

    //使用VMXon 汇编进行打开.
    Asm_VmxOn(g_CpuGlobalValueInfo.VmOnxRegionPA.LowPart, g_CpuGlobalValueInfo.VmOnxRegionPA.HighPart);

    //检测EFLAGS位置.
    *((PULONG)&eflags) = Asm_GetEflags();
    if (eflags.CF != 0)
    {
        KdPrint(("调用VMX_ON失败,检测到CF位不等于0 \r\n"));
        ExFreePool(g_CpuGlobalValueInfo.VmxOnRegion);
        return STATUS_UNSUCCESSFUL;
    }

    KdPrint(("开启VMXON成功,进入VMX host模式\r\n"));
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI TrunOffVmxOff()
{
    
    KdPrint(("关闭VT模式中,请查看输出\r\n"));
    Asm_VmxOff();
    if (NULL != g_CpuGlobalValueInfo.VmxOnRegion)
    {
        ExFreePool(g_CpuGlobalValueInfo.VmxOnRegion);
        KdPrint(("释放VmxOn内存成功\r\n"));
    }

    //关闭CR4寄存器.
    if (!NT_SUCCESS(TurnOnTheCr4(FALSE)))
    {
        KdPrint(("VT Cr4为1 无法设置,关闭失败\r\n"));
        return STATUS_UNSUCCESSFUL;
    }

   

    KdPrint(("关闭VT成功,已成功关闭\r\n"));
    return STATUS_SUCCESS;
}