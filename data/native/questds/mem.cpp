////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   mem.cpp
//  Version:     v1.00
//  Created:     20/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#include "mem.h"
#include "stdlib.h"

void* dalloc(size_t    size)
{
    return malloc(size);
}

void* dcalloc(size_t size, size_t count)
{
    int total_size = count * size;
    void* p = malloc(total_size);
    if (p) {
        dset(p, 0, total_size);
    
    }

    return p;
}

void* drealloc(void* p, size_t new_size)
{
    return realloc(p, new_size);
}

void dfree(void* p)
{
    free(p);
}

void* dcpy(void* Dest, const void* Source, size_t Size) {
    size_t i;

    if ((uintptr_t) Dest % sizeof(long) == 0 && (uintptr_t) Source % sizeof(long) == 0 && Size % sizeof(long) == 0) {
        long* DIt = (long*)Dest;
        const long* SIt = (const long*)Source;

        for (i = 0; i < Size / sizeof(long); i++) {
            DIt[i] = SIt[i];

        }

    }
    else {
        char* DIt = (char*)Dest;
        const char* SIt = (const char*)Source;

        for (i = 0; i < Size; i++) {
            DIt[i] = SIt[i];

        }

    }

    return Dest;

}

void* dset(void* Target, u8 Value, size_t Size) {
    unsigned char *ptr = (unsigned char*)Target;
    while (Size-- > 0)
    *ptr++ = Value;
    return Target;

}

void* dmove(void* Dest, const void* Source, size_t Size) {
    char* DIt = (char*)Dest;
    const char* SIt = (const char*)Source;

    if (DIt < SIt) {
        while (Size--) {
            *DIt++ = *SIt++;

        }

    }
    else {
        char* LastS = (char*)SIt + (Size - 1);
        char* LastD = DIt + (Size - 1);

        while (Size--) {
            *LastD-- = *LastS--;

        }

    }

    return Dest;

}

u32 dpow2(u32 In) {
    In--;
    In |= In >> 1;
    In |= In >> 2;
    In |= In >> 4;
    In |= In >> 8;
    In |= In >> 16;
    In++;
    return In;

}
