
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
