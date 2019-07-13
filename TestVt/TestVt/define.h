#pragma once
#include "public.h"

#define IA32_FEATURE_CONTROL 0x3A
#define CR4_VMX_Model        0x2000 //设置CR4寄存器的第14位为1,开启VMX模式.

typedef struct  
{
    PVOID VmxOnRegion;   //VMON申请的虚拟地址
    PHYSICAL_ADDRESS  VmOnxRegionPA; //VMX的物理地址
   
}G_CPU,*PG_CPU; //记录全局变量的信息

typedef union 
{
    struct 
    {
        ULONG CF : 1;
        ULONG Reserve1 : 1;
        ULONG PF : 1;
        ULONG Reserve2 : 1;
        ULONG AF : 1;
        ULONG Reserve3 : 1;
        ULONG ZF : 1;
        ULONG SF : 1;
        ULONG TF : 1;
        ULONG LF : 1;
        ULONG DF : 1;
        ULONG OF : 1;
        ULONG IOPL : 2;
        ULONG NT : 1;
        ULONG Reserve4 : 1;
        ULONG RF : 1;
        ULONG VM : 1;
        ULONG AC : 1;
        ULONG VIF : 1;
        ULONG VIP : 1;
        ULONG ID : 1;
        ULONG Reserve5 : 10;
    };
   
   
}EFLAGS,*PEFLAGS;

//CRO的结构定义
typedef union 
{
    struct 
    {
        ULONG PE : 1;
        ULONG MP : 1;
        ULONG EM : 1;
        ULONG TS : 1;
        ULONG ET : 1;
        ULONG NE : 1;
        ULONG Reserve1 : 10;
        ULONG WP : 1;
        ULONG Reserve2 : 1;
        ULONG AM : 1;
        ULONG Reserve3 : 10;
        ULONG NW : 1;
        ULONG CD : 1;
        ULONG PG : 1;
    };
}CR0_FLAGS,*PCRO_FLAGS;

typedef union
{
    struct 
    {
        ULONG Reserve : 32;
    };
}CR1_FLAGS,*PCR1_FLAGS;

typedef union
{
    struct
    {
        ULONG Reserve : 32;
    };
}CR2_FLAGS, *PCR2_FLAGS;


typedef union
{
    struct
    {
        ULONG Reserve1 : 2;
        ULONG PWT : 1;
        ULONG PCD : 1;
        ULONG Reserve2 : 7;
        ULONG PageDirBase : 20;
    };
}CR3_FLAGS, *PCR3_FLAGS;

//CR4寄存器代表的意思
typedef union
{
    struct
    {
        ULONG VME : 1;
        ULONG PVI : 1;
        ULONG TSD : 1;
        ULONG DE : 1;
        ULONG PSE : 1;
        ULONG PAE : 1;
        ULONG MCE : 1;
        ULONG PGE : 1;
        ULONG PCE : 1;
        ULONG OSFXSR : 1;
        ULONG OSXMMEXCPT : 1;
        ULONG Reserve1 : 2;
        ULONG VMXE : 1;
        ULONG SMXE : 1;
        ULONG PCIDE : 1;
        ULONG OSXSAVE : 1;
        ULONG Reserve2 : 1;
        ULONG SMEP : 1;
        ULONG Reserve3 : 12;
    };
}CR4_FLAGS, *PCR4_FLAGS;

