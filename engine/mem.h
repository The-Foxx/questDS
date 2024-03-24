////////////////////////////////////////////////////////////////////////////
//
//  QuestDS Engine Source File.
//  Copyright (C) 2024 Clara Lille
// -------------------------------------------------------------------------
//  File name:   mem.h
//  Version:     v1.00
//  Created:     20/03/24 by Clara.
//  Description: 
// -------------------------------------------------------------------------
//  This project is licensed under the MIT License
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "base_types.h"

void* dalloc(size_t size);
void* dcalloc(size_t size, size_t count);
void* drealloc(void* p, size_t new_size);
void dfree(void* p);
void* dcpy(void* Dest, const void* Source, size_t Size);
void* dset(void* Target, u8 Value, size_t Size);
void* dmove(void* Dest, const void* Source, size_t Size);
u32 dpow2(u32 In);

namespace DS {
    template <typename iType> struct dmem {
    private:

        u32 ArraySize;
        u32 Allocation;
        iType* Ptr;

        #define MYTH_DEFAULT_DMEM_SIZE 8
        inline void internalInit(u32 Size) {
            ArraySize = 0;
            Allocation = dpow2(Size);
            if (Allocation < MYTH_DEFAULT_DMEM_SIZE) {
                Allocation = MYTH_DEFAULT_DMEM_SIZE;

            }

            Ptr = (iType*)dalloc(Allocation * sizeof(iType));

        }

    public:

        inline void checkSize(u32 Size) {
            //printf("check size %u times %lu\n", Size, sizeof(Size));
            u32 NewSize = dpow2(Size);
            if (NewSize < MYTH_DEFAULT_DMEM_SIZE) {
                NewSize = MYTH_DEFAULT_DMEM_SIZE;

            }
            if (NewSize != Allocation) {
                //printf("Allocating for %u newSize %u alloc %u\n", Size, NewSize, Allocation);
                Ptr = (iType*)drealloc(Ptr, NewSize * sizeof(iType));
                Allocation = NewSize;

            }

        }

        typedef void (*loopFn)(iType &, u64);

        inline void loopAll(loopFn UsrFunc) {
            for (u64 i = 0; i < ArraySize; i++) {
                UsrFunc(Ptr[i], i);

            }

        }

        inline void loopAllHeap(loopFn UsrFunc) {
            for (u64 i = 0; i < Allocation; i++) {
                UsrFunc(Ptr[i], i);

            }

        }

        inline void init() {
            internalInit(MYTH_DEFAULT_DMEM_SIZE);

        }

        inline void init(u32 Size) {
            internalInit(Size);

        }

        inline dmem() {
            internalInit(MYTH_DEFAULT_DMEM_SIZE);
        }

        inline dmem(u32 Size) {
            internalInit(Size);

        }

        inline dmem(dmem<iType> const& Rhs) {
            init(Rhs.Allocation);
            ArraySize = Rhs.ArraySize;
            Ptr = (iType*)dcpy(Ptr, Rhs.Ptr, Allocation * sizeof(iType));

        }

        inline iType* data() {
            return Ptr;

        }

        inline iType* last(){
            if (ArraySize == 0) {
                return NULL;

            }

            return Ptr + (ArraySize - 1);

        }

        inline iType* lastCopy(){
            if (ArraySize == 0) {
                return iType{};

            }

            return *(Ptr + (ArraySize - 1));

        }

        inline u32 size() {
            return ArraySize;

        }

        inline void setSize(u32 Rhs) {
            ArraySize = Rhs;

        }

        inline u32 alloc() {
            return Allocation;

        }

        inline void append(iType const& In) {
            checkSize(ArraySize + 1);
            *(Ptr + ArraySize) = In;
            ArraySize++;

        }

        inline void appendMem(const iType* In) {
            checkSize(ArraySize + 1);
            dcpy((Ptr + ArraySize), In, sizeof(iType));
            ArraySize++;

        }

        inline void appendNull() {
            checkSize(ArraySize + 1);
            dset(Ptr + ArraySize, 0, sizeof(iType));
            ArraySize++;

        }

        inline void zeroOut() {
            dset(Ptr, 0, Allocation * sizeof(iType));

        }

        inline void clear() {
            ArraySize = 0;

        }

        inline iType getCopy(u32 Index) {
            return *(Ptr + Index);

        }

        inline iType* getPtr(u32 Index) {
            return Ptr + Index;

        }

        inline void empty() {
            checkSize(MYTH_DEFAULT_DMEM_SIZE);
            ArraySize = 0;
            dset((void*)Ptr, 0, Allocation);

        }

    };

}