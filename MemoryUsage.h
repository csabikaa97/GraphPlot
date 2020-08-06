//
//  MemoryUsage.h
//  GraphPlot
//
//  Created by Csaba on 3/22/20.
//  Copyright © 2020 Csaba. All rights reserved.
//

#ifndef MemoryUsage_h
#define MemoryUsage_h

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstdint>

const char MemoryUsageHandle[] = "[ MemoryStats";
bool initialized = false;
int NumberOfSessions = 0;

class MemoryUsageStats {
private:
    int CurrentAllocations;
    int MaxAllocations;
    int token;
    
    unsigned long int CurrentSize;
    unsigned long int MaxSize;
    
    bool PrintDebugMessages;
    
    void** Pointers;
    size_t* Sizes;
    int* Tokens;
    bool* block;
    int CurrentPointerCapacity;
public:
    MemoryUsageStats(bool PrintDebugMessages) {
        token = 0;
        CurrentAllocations = 0;
        CurrentSize = 0;
        MaxAllocations = 0;
        MaxSize = 0;
        this->PrintDebugMessages = PrintDebugMessages;
        initialized = true;
        Pointers = (void**)malloc(sizeof(void*)*10);
        Sizes = (size_t*)malloc(sizeof(size_t)*10);
        CurrentPointerCapacity = 10;
        Tokens = (int*)malloc(sizeof(int)*10);
        block = (bool*)malloc(sizeof(bool)*10);
    }
    ~MemoryUsageStats() {
        Ki();
        initialized = false;
    }
    void Ki();
    friend void* operator new(size_t Meret);
    friend void* operator new[] (size_t Meret);
    friend void operator delete(void* ptr) noexcept;
    friend std::ostream& operator<<(std::ostream& folyam, MemoryUsageStats Kiirando);
    friend void operator delete(void* FelszabaditandoPointer) noexcept;
};

MemoryUsageStats* A;

void MemoryUsageStats::Ki() {
    printf("%s session%d: current allocations: %d\tmax allocations: %d ]\n", MemoryUsageHandle, NumberOfSessions, A->CurrentAllocations, A->MaxAllocations);
    printf("%s session%d: current usage %zu B\tmax usage: %zu B ]\n", MemoryUsageHandle, NumberOfSessions, A->CurrentSize, A->MaxSize);
    if(CurrentAllocations > 0) {
        printf("%s session%d: Records: \n", MemoryUsageHandle, NumberOfSessions);
        for(int i=0; i<A->CurrentAllocations; i++) {
            printf("\t%p\n", A->Pointers[i]);
        }
        printf("\n");
    }
    printf("\n");
}

void* operator new(size_t Meret) {
    if(initialized) {
        if(A->CurrentAllocations == A->CurrentPointerCapacity) {
            A->CurrentPointerCapacity = A->CurrentPointerCapacity + 10;
            A->Pointers = (void**)realloc(A->Pointers, A->CurrentPointerCapacity);
            A->Sizes = (size_t*)realloc(A->Sizes, A->CurrentPointerCapacity);
            A->Tokens = (int*)realloc(A->Tokens, A->CurrentPointerCapacity);
            A->block = (bool*)realloc(A->block, A->CurrentPointerCapacity);
        }
        A->Pointers[A->CurrentAllocations] = (void*)malloc(Meret);
        A->Sizes[A->CurrentAllocations] = Meret;
        A->block[A->CurrentAllocations] = false;
        A->Tokens[A->CurrentAllocations] = A->token;
        if(A->PrintDebugMessages) {
            printf("%s session%d: Allocating memory slot\t%d\t(%p) with size %zu B ]\n", MemoryUsageHandle, NumberOfSessions, A->token, A->Pointers[A->CurrentAllocations], Meret);
        }
        A->token = A->token + 1;
        
        A->CurrentAllocations = A->CurrentAllocations + 1;
        if(A->CurrentAllocations > A->MaxAllocations) {
            A->MaxAllocations = A->CurrentAllocations;
        }
        A->CurrentSize = A->CurrentSize + Meret;
        if(A->CurrentSize > A->MaxSize) {
            A->MaxSize = A->CurrentSize;
        }
        return A->Pointers[A->CurrentAllocations - 1];
    } else {
        return malloc(Meret);
    }
}

/*void* operator new[] (size_t Meret) {
    if(initialized) {
        A->Pointers[A->CurrentAllocations] = (void*)malloc(Meret);
        if(A->CurrentAllocations == A->CurrentPointerCapacity) {
            A->CurrentPointerCapacity = A->CurrentPointerCapacity + 10;
            A->Pointers = (void**)realloc(A->Pointers, A->CurrentPointerCapacity);
            A->Sizes = (size_t*)realloc(A->Sizes, A->CurrentPointerCapacity);
            A->Tokens = (int*)realloc(A->Tokens, A->CurrentPointerCapacity);
            A->block = (bool*)realloc(A->block, A->CurrentPointerCapacity);
        }
        A->Sizes[A->CurrentAllocations] = Meret;
        A->block[A->CurrentAllocations] = true;
        A->Tokens[A->CurrentAllocations] = A->token;
        if(A->PrintDebugMessages) {
            printf("%s session%d: Allocating memory block\t%d\t(%p) with size %zu B ]\n", MemoryUsageHandle, NumberOfSessions, A->token, A->Pointers[A->CurrentAllocations], Meret);
        }
        A->token = A->token + 1;
        
        A->CurrentAllocations = A->CurrentAllocations + 1;
        if(A->CurrentAllocations > A->MaxAllocations) {
            A->MaxAllocations = A->CurrentAllocations;
        }
        A->CurrentSize = A->CurrentSize + Meret;
        if(A->CurrentSize > A->MaxSize) {
            A->MaxSize = A->CurrentSize;
        }
        return A->Pointers[A->CurrentAllocations - 1];
    } else {
        return malloc(Meret);
    }
}*/

void operator delete(void* FelszabaditandoPointer) noexcept {
    if(initialized) {
        int i = 0;
        while( i < A->CurrentAllocations && A->Pointers[i] != FelszabaditandoPointer ) {
            i = i + 1;
        }
        if(i == A->CurrentAllocations) {
            if(A->PrintDebugMessages) {
                printf("%s session%d: Freeing unknown memory block (%p) ]\n", MemoryUsageHandle, NumberOfSessions, FelszabaditandoPointer);
            }
        } else {
            if(A->PrintDebugMessages) {
                printf("%s session%d: Freeing memory block\t%d\t(%p)\tsize: %zu B ]\n", MemoryUsageHandle, NumberOfSessions, A->Tokens[i], FelszabaditandoPointer, A->Sizes[i]);
            }
            A->CurrentAllocations = A->CurrentAllocations - 1;
            A->CurrentSize = A->CurrentSize - A->Sizes[i];
            A->Pointers[i] = A->Pointers[A->CurrentAllocations];
            A->Sizes[i] = A->Sizes[A->CurrentAllocations];
            A->Tokens[i] = A->Tokens[A->CurrentAllocations];
        }
    }
    free(FelszabaditandoPointer);
}

#endif /* MemoryUsage_h */
