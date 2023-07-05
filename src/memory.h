#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>
#include <assert.h>

#define WORD_SIZE sizeof(intptr_t)

#ifndef MEM_ALLOC
#include <stdlib.h>
#define MEM_ALLOC(size) malloc((size))
#endif

#ifdef MEM_DEBUG
#include <stdio.h>
#endif

typedef struct
{
    size_t capacity;
    size_t size;
    char *data;
} Region;

Region alloc_region(size_t capacity);
#define reset_region(region) ((region)->size = 0)
void *ralloc(Region *region, size_t size);

#ifdef MEM_IMPLEMENTATION

// Allocate region
Region alloc_region(size_t capacity)
{
    Region region = {capacity, 0, MEM_ALLOC(capacity)};
    return region;
}

// Allocate memory in region rounding up to word size
void *ralloc(Region *region, size_t size)
{
    size_t aligned_size = (size + WORD_SIZE - 1) & ~(WORD_SIZE - 1);
    assert(region->size + aligned_size <= region->capacity);
    //     if (region->size + aligned_size > region->capacity)
    //     {
    // #ifdef MEM_DEBUG
    //         printf("Region reallocated\n");
    // #endif
    //         region->capacity = region->capacity * 2 + aligned_size;
    //         region->data = MEM_ALLOC(region->capacity);
    //     }
    void *ptr = region->data + region->size;
    region->size += aligned_size;
    return ptr;
}

#endif
#endif
