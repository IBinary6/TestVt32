
# 2019-7-17 explain date
1.Set up the VMCS host domain

2.Set up the VMCS Control domain

3.Add _VMX_BASIC_MSR struct

4.Change IsSupportedVmx function the susport CR0.NE check

5.Change TrunOnVmxOn  function Setting
g_CpuGlobalValueInfo.VmxOnRegion vesion 

6.Modify the TurnOnVmxOn function. For the assignment of the version number set initiated by g_CpuGlobalValueInfo.VmxOnRegion, read the MSR_BASIC register directly.

7.Modify the VmcsSupport function. Increase the setting of the g_CpuGlobalValueInfo.pVmxVmcsRegion version number. Read MSR_BASIC register directly

8.The assembly of the assembly method, cs ss ed ds gs fs tr idtr gdtr and the entry of the Host domain are added.

# 2019-7-15 explain date
1.add three function as
  VmcsSupport(); 
  SetVmcs();
  SetControlBitValue(); 
2.add three Assembly function
   Asm_VmLaunch();  //start vmx model
   Asm_VmRead();    //read vmcs
   Asm_VmWrite();   //write vmcs
3.Add something new structure to the enum
  MSR register   0x481 
  MSR register   0x482

# 2019-7-13 explain date

1.support start VMXON VMXOFF

2.And define New Struct CR0 - CR4-EFLAGS

3.VT Check support function Add  CRO.PG CRO.PE check
