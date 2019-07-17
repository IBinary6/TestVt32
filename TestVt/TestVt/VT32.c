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
    if (Cr0.PE != 1 || Cr0.PG != 1 ||Cr0.NE != 1)
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
    VMX_BASIC_MSR msr;
    ULONG uRevid;
    RtlZeroMemory(&msr, sizeof(msr));
    *(PULONG)&msr = (ULONG)Asm_ReadMsr32(MSR_IA32_VMX_BASIC);

    g_CpuGlobalValueInfo.VmxOnRegion = ExAllocatePoolWithTag(NonPagedPool, 0x1000, 'XMV');
    if (NULL == g_CpuGlobalValueInfo.VmxOnRegion)
    {
        KdPrint(("��VMXONʧ��\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    RtlZeroMemory(g_CpuGlobalValueInfo.VmxOnRegion, 0x1000);
    g_CpuGlobalValueInfo.VmOnxRegionPA = MmGetPhysicalAddress(g_CpuGlobalValueInfo.VmxOnRegion);

    uRevid = msr.RevId;
    *(ULONG*)g_CpuGlobalValueInfo.VmxOnRegion = uRevid;
    //*(ULONG*)g_CpuGlobalValueInfo.pVmxVmcsRegion = uRevid;


   // RtlZeroMemory(((void *)(&g_CpuGlobalValueInfo.VmOnxRegionPA)), sizeof(PHYSICAL_ADDRESS));
    KdPrint(("�汾����Ϣ = [%p]", uRevid));

    
    //Ҫ���ð汾��.���������. 00001000 0000 0001 ���4���ֽ�����Ϊ1
   // RtlZeroMemory(g_CpuGlobalValueInfo.VmxOnRegion, 0x1000);
    //*(PULONG)g_CpuGlobalValueInfo.VmxOnRegion = 1;
    //��Ҫ���������ַ.���Դ��������ַ.
   
    //TurnOnTheCr4(FALSE);
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


NTSTATUS NTAPI VmcsSupport()
{
    EFLAGS flags;

    ULONG uRevid;
    VMX_BASIC_MSR msr;
    g_CpuGlobalValueInfo.pVmxVmcsRegion = NULL;
    g_CpuGlobalValueInfo.pVmxVmcsRegion = ExAllocatePoolWithTag(NonPagedPool, 0x1000, 'XMV');
    if (NULL == g_CpuGlobalValueInfo.pVmxVmcsRegion)
    {
        KdPrint(("VMCS�����ַ�ռ�����ʧ��\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    //ת�������ַ
    KdPrint(("Vmcs����ռ�����ɹ�\r\n"));
    RtlZeroMemory(g_CpuGlobalValueInfo.pVmxVmcsRegion, 0x1000);
    
    RtlZeroMemory(&msr, sizeof(msr));
    *(PULONG)&msr = (ULONG)Asm_ReadMsr32(MSR_IA32_VMX_BASIC);

    *(PULONG)g_CpuGlobalValueInfo.pVmxVmcsRegion = msr.RevId;

    g_CpuGlobalValueInfo.pVmxVmcsRegionPA = MmGetPhysicalAddress(g_CpuGlobalValueInfo.pVmxVmcsRegion);
    //ʹ��vmclear���
    
    Asm_VmClear(g_CpuGlobalValueInfo.pVmxVmcsRegionPA.LowPart, g_CpuGlobalValueInfo.pVmxVmcsRegionPA.HighPart);
    //ʹ��vmptrldװ��.
    *((PULONG)&flags) = Asm_GetEflags();
    if (flags.CF != 0 && flags.ZF != 0)
    {
        KdPrint(("ִ��VmClearʧ��\r\n"));
        return STATUS_UNSUCCESSFUL;
    }
    KdPrint(("ʹ��VmClear�ɹ�,���������״̬Ϊδ����\r\n"));
    Asm_VmPtrld(g_CpuGlobalValueInfo.pVmxVmcsRegionPA.LowPart, g_CpuGlobalValueInfo.pVmxVmcsRegionPA.HighPart);

    return STATUS_SUCCESS;
}

/*
* �����ǰ���Intel�������ֲ�������VMCS����������ɲ���������д
* VMCS��������6����ɲ��֣��ֱ���*
* 1.�ͻ���״̬��Guest State Area��
* 2.������״̬��Host State Area��
* 3.�����ִ�п�����VM-Execuction Control Fields��
* 4.VMEntry��Ϊ������VM-Entry Control Fields��
* 5.VMExit��Ϊ������VM-Exit Control Fields��
* 6.VMExit�����Ϣ��VM-Exit Information Fields����ֻ����
*/

NTSTATUS NTAPI SetVcms()
{
    NTSTATUS ntStatus;
    KdPrint(("��ʼ����Vmcs \r\n"));
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

//��ʼ��HostVmcs��Ϣ
NTSTATUS ControlVmcsInit()
{
    //vmx control 
      //3.1 vm ִ�п���
    Asm_VmWrite(PIN_BASED_VM_EXEC_CONTROL, SetControlBitValue(0, IA32_VMX_PINBASED_CTLS));
    //����ִ�пؼ�.
    Asm_VmWrite(CPU_BASED_VM_EXEC_CONTROL, SetControlBitValue(0, IA32_VMX_PROCBASED_CTLS));

    Asm_VmWrite(VM_ENTRY_CONTROLS, SetControlBitValue(0, IA32_VMX_ENTRY_CTLS));
    Asm_VmWrite(VM_EXIT_CONTROLS, SetControlBitValue(0, IA32_VMX_EXIT_CTLS));
    //vm exit����
    //vmentry ����.
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
        KdPrint(("�Բ���,VMCS��Ϣ����Host��ջ����,��������ռ�\r\n"));
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
    //��Ϊ����80λ���ɼ�����,�ֱ��Ƕν���.�Լ���base.������Ҫ��д.������дBase�ֶ�
    //��дhost��ַ.
    Asm_VmWrite(HOST_FS_BASE, GetSegmentDescriptorBase(GdtBase, Asm_GetFs()));;
    Asm_VmWrite(HOST_GS_BASE, GetSegmentDescriptorBase(GdtBase, Asm_GetGs()));
    KdBreakPoint();
    Asm_VmWrite(HOST_TR_BASE, GetSegmentDescriptorBase(GdtBase, Asm_GetTr()));
    //��дGDTR/IDTR�� Base
    Asm_VmWrite(HOST_GDTR_BASE,GdtBase);
    Asm_VmWrite(HOST_IDTR_BASE, IdtBase);

    //����CS ESP EIP
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
        KdPrint(("�Բ���,VMCS��Ϣ����Guest��ջ����,��������ռ�\r\n"));
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

// ��ȡ����������ַ
ULONG
GetSegmentDescriptorBase(
    ULONG gdt_base,
    USHORT seg_selector)
{
    ULONG Base = 0;
    SEGMENT_DESCRIPTOR	SegDescriptor = { 0 };

    //
    // ��GDT����������������
    //
    RtlCopyMemory(&SegDescriptor, (ULONG *)(gdt_base + (seg_selector >> 3) * 8), 8);

    //
    // ���������ĸ�8λ
    //
    Base = SegDescriptor.BaseHi;
    Base <<= 8;

    //
    // ����������31-39λ
    //
    Base |= SegDescriptor.BaseMid;
    Base <<= 16;

    //
    // ���������� 15-31λ
    //
    Base |= SegDescriptor.BaseLo;

    return Base;
}

