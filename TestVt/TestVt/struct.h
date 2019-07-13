#pragma once
#include "public.h"

typedef  union
{

    struct
    {
        unsigned SSE3 : 1;
        unsigned PCLMULQD1 : 1;
        unsigned DTES64 : 1;
        unsigned MONITOR : 1;
        unsigned DS_CPL : 1;
        unsigned VMX : 1;
        unsigned SMX : 1;
        unsigned TM2 : 1;
        unsigned SSSE3 : 1;
        unsigned Reserved : 1;

    };

}CPUID_ECX, *PCUPID_ECX;