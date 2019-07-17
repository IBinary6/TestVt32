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
    if (Cr0.PE != 1 || Cr0.PG != 1 ||Cr0.NE != 1)
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
    VMX_BASIC_MSR msr;
    ULONG uRevid;
    RtlZeroMemory(&msr, sizeof(msr));
    *(PULONG)&msr = (ULONG)Asm_ReadMsr32(MSR_IA32_VMX_BASIC);

    g_CpuGlobalValueInfo.VmxOnRegion = ExAllocatePoolWithTag(NonPagedPool, 0x1000, 'XMV');
    if (NULL == g_CpuGlobalValueInfo.VmxOnRegion)
    {
        KdPrint(("打开VMXON失败\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    RtlZeroMemory(g_CpuGlobalValueInfo.VmxOnRegion, 0x1000);
    g_CpuGlobalValueInfo.VmOnxRegionPA = MmGetPhysicalAddress(g_CpuGlobalValueInfo.VmxOnRegion);

    uRevid = msr.RevId;
    *(ULONG*)g_CpuGlobalValueInfo.VmxOnRegion = uRevid;
    //*(ULONG*)g_CpuGlobalValueInfo.pVmxVmcsRegion = uRevid;


   // RtlZeroMemory(((void *)(&g_CpuGlobalValueInfo.VmOnxRegionPA)), sizeof(PHYSICAL_ADDRESS));
    KdPrint(("版本号信息 = [%p]", uRevid));

    
    //要设置版本号.否则会蓝屏. 00001000 0000 0001 最后4个字节设置为1
   // RtlZeroMemory(g_CpuGlobalValueInfo.VmxOnRegion, 0x1000);
    //*(PULONG)g_CpuGlobalValueInfo.VmxOnRegion = 1;
    //需要传入物理地址.所以传入物理地址.
   
    //TurnOnTheCr4(FALSE);
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


NTSTATUS NTAPI VmcsSupport()
{
    EFLAGS flags;

    ULONG uRevid;
    VMX_BASIC_MSR msr;
    g_CpuGlobalValueInfo.pVmxVmcsRegion = NULL;
    g_CpuGlobalValueInfo.pVmxVmcsRegion = ExAllocatePoolWithTag(NonPagedPool, 0x1000, 'XMV');
    if (NULL == g_CpuGlobalValueInfo.pVmxVmcsRegion)
    {
        KdPrint(("VMCS虚拟地址空间申请失败\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //转化物理地址
    KdPrint(("Vmcs虚拟空间申请成功\r\n"));
    RtlZeroMemory(g_CpuGlobalValueInfo.pVmxVmcsRegion, 0x1000);
    
    RtlZeroMemory(&msr, sizeof(msr));
    *(PULONG)&msr = (ULONG)Asm_ReadMsr32(MSR_IA32_VMX_BASIC);

    *(PULONG)g_CpuGlobalValueInfo.pVmxVmcsRegion = msr.RevId;

    g_CpuGlobalValueInfo.pVmxVmcsRegionPA = MmGetPhysicalAddress(g_CpuGlobalValueInfo.pVmxVmcsRegion);
    //使用vmclear清空
    
    Asm_VmClear(g_CpuGlobalValueInfo.pVmxVmcsRegionPA.LowPart, g_CpuGlobalValueInfo.pVmxVmcsRegionPA.HighPart);
    //使用vmptrld装载.
    *((PULONG)&flags) = Asm_GetEflags();
    if (flags.CF != 0 && flags.ZF != 0)
    {
        KdPrint(("执行VmClear失败\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    KdPrint(("使用VmClear成功,设置虚拟机状态为未激活\r\n"));
    Asm_VmPtrld(g_CpuGlobalValueInfo.pVmxVmcsRegionPA.LowPart, g_CpuGlobalValueInfo.pVmxVmcsRegionPA.HighPart);

    return STATUS_SUCCESS;
}

/*
* 这里是按照Intel开发者手册描述的VMCS的数据区组成部分依次填写
* VMCS数据区有6个组成部分，分别是*
* 1.客户区状态域（Guest State Area）
* 2.宿主机状态域（Host State Area）
* 3.虚拟机执行控制域（VM-Execuction Control Fields）
* 4.VMEntry行为控制域（VM-Entry Control Fields）
* 5.VMExit行为控制域（VM-Exit Control Fields）
* 6.VMExit相关信息域（VM-Exit Information Fields）（只读）
*/

NTSTATUS NTAPI SetVcms()
{
    NTSTATUS ntStatus;
    KdPrint(("开始设置Vmcs \r\n"));
    //GUEST state
    /*ntStatus =  GuestVmcsInit();
    if (!NT_SUCCESS(ntStatus))
        return STATUS_UNSUCCESSFUL;*/
    //host state
    ntStatus = HostVmcsInit();
    if (!NT_SUCCESS(ntStatus))
        return STATUS_UNSUCCESSFUL;

    //vm control 
    ntStatus = ControlVmcsInit();
    if (!NT_SUCCESS(ntStatus))
        return STATUS_UNSUCCESSFUL;

 
    return STATUS_SUCCESS;
}

static ULONG SetControlBitValue(ULONG Ctl, ULONG Msr)
{
    LARGE_INTEGER lar;
    lar.QuadPart = Asm_ReadMsr32(Msr);
    Ctl &= lar.HighPart;
    Ctl |= lar.LowPart;
    return Ctl;
}

//初始化HostVmcs信息
NTSTATUS ControlVmcsInit()
{
    //vmx control 
      //3.1 vm 执行控制
    Asm_VmWrite(PIN_BASED_VM_EXEC_CONTROL, SetControlBitValue(0, IA32_VMX_PINBASED_CTLS));
    //设置执行控件.
    Asm_VmWrite(CPU_BASED_VM_EXEC_CONTROL, SetControlBitValue(0, IA32_VMX_PROCBASED_CTLS));

    Asm_VmWrite(VM_ENTRY_CONTROLS, SetControlBitValue(0, IA32_VMX_ENTRY_CTLS));
    Asm_VmWrite(VM_EXIT_CONTROLS, SetControlBitValue(0, IA32_VMX_EXIT_CTLS));
    //vm exit控制
    //vmentry 控制.
    return STATUS_SUCCESS;
}
NTSTATUS HostVmcsInit()
{
    ULONG64 Msr;
    ULONG GdtBase = 0;
    ULONG IdtBase = 0;
    GdtBase = Asm_GetGdtrBase();
    g_CpuGlobalValueInfo.pHostStack = NULL;
    g_CpuGlobalValueInfo.pHostStack = ExAllocatePoolWithTag(NonPagedPool, 0x8000, 'kcts');
    GdtBase = Asm_GetGdtrBase();
    IdtBase = Asm_GetIdtBase();
    if (NULL == g_CpuGlobalValueInfo.pHostStack)
    {
        KdPrint(("对不起,VMCS信息设置Host堆栈出错,请检查申请空间\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    RtlZeroMemory(g_CpuGlobalValueInfo.pHostStack, 0x8000);

    KdPrint(("pHostStack = %p \r\n", g_CpuGlobalValueInfo.pHostStack));
    KdBreakPoint();
    Asm_VmWrite(HOST_CR0,Asm_GetCr0());
    Asm_VmWrite(HOST_CR3,Asm_GetCr3());
    Asm_VmWrite(HOST_CR4,Asm_GetCr432());

    Asm_VmWrite(HOST_ESP, ((ULONG)((PUCHAR)(g_CpuGlobalValueInfo.pHostStack) + 0x7000)));
    Asm_VmWrite(HOST_EIP, (ULONG)HostVmEntry);
    KdPrint(("Host Entry Address = %p \r\n", &HostVmEntry));
    ;//CS SS DS ES FS GS TR
    Asm_VmWrite(HOST_CS_SELECTOR,Asm_GetCs() & 0xFFF8);
    Asm_VmWrite(HOST_SS_SELECTOR,Asm_GetSs() & 0xFFF8);
    Asm_VmWrite(HOST_DS_SELECTOR,Asm_GetDs() & 0xFFF8);
    Asm_VmWrite(HOST_ES_SELECTOR,Asm_GetEs() & 0xFFF8);
    Asm_VmWrite(HOST_FS_SELECTOR,Asm_GetFs() & 0xFFF8);
    Asm_VmWrite(HOST_GS_SELECTOR,Asm_GetGs() & 0xFFF8);
    Asm_VmWrite(HOST_TR_SELECTOR,Asm_GetTr() & 0xFFF8);
    //因为段有80位不可见部分,分别是段界限.以及段base.所以需要填写.首先填写Base字段
    //填写host基址.
    Asm_VmWrite(HOST_FS_BASE, GetSegmentDescriptorBase(GdtBase, Asm_GetFs()));;
    Asm_VmWrite(HOST_GS_BASE, GetSegmentDescriptorBase(GdtBase, Asm_GetGs()));
    KdBreakPoint();
    Asm_VmWrite(HOST_TR_BASE, GetSegmentDescriptorBase(GdtBase, Asm_GetTr()));
    //填写GDTR/IDTR的 Base
    Asm_VmWrite(HOST_GDTR_BASE,GdtBase);
    Asm_VmWrite(HOST_IDTR_BASE, IdtBase);

    //设置CS ESP EIP
    Asm_VmWrite(HOST_IA32_SYSENTER_CS, Asm_ReadMsr32(MSR_IA32_SYSENTER_CS) & 0xFFFFFFFF);
    Asm_VmWrite(HOST_IA32_SYSENTER_ESP, Asm_ReadMsr32(MSR_IA32_SYSENTER_ESP) & 0xFFFFFFFF);
    Asm_VmWrite(HOST_IA32_SYSENTER_EIP, Asm_ReadMsr32(MSR_IA32_SYSENTER_EIP) & 0xFFFFFFFF); // 


   
  
   

    return STATUS_SUCCESS;
}

NTSTATUS GuestVmcsInit()
{
    g_CpuGlobalValueInfo.pGuestStack = NULL;
    g_CpuGlobalValueInfo.pGuestStack = ExAllocatePoolWithTag(NonPagedPool, 0x8000, 'kcts');
    if (NULL == g_CpuGlobalValueInfo.pHostStack)
    {
        KdPrint(("对不起,VMCS信息设置Guest堆栈出错,请检查申请空间\r\n"));
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

// 获取段描述符基址
ULONG
GetSegmentDescriptorBase(
    ULONG gdt_base,
    USHORT seg_selector)
{
    ULONG Base = 0;
    SEGMENT_DESCRIPTOR	SegDescriptor = { 0 };

    //
    // 从GDT中索引到段描述符
    //
    RtlCopyMemory(&SegDescriptor, (ULONG *)(gdt_base + (seg_selector >> 3) * 8), 8);

    //
    // 段描述符的高8位
    //
    Base = SegDescriptor.BaseHi;
    Base <<= 8;

    //
    // 段描述符的31-39位
    //
    Base |= SegDescriptor.BaseMid;
    Base <<= 16;

    //
    // 段描述符的 15-31位
    //
    Base |= SegDescriptor.BaseLo;

    return Base;
}

