
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <reent.h>

#include "uvisor-lib/uvisor-lib.h"

#define DPRINTF(...) {};
/* Use printf with caution inside malloc: printf may allocate memory, so
   using printf in malloc may lead to recusive calls! */
/* #define DPRINTF(fmt, ...) printf(fmt, ## __VA_ARGS__) */

static UvisorAllocator malloc_allocator = NULL;

UVISOR_WEAK UvisorAllocator uvisor_allocator_init(void)
{
    /* Use our own allocator within the process heap space */
    extern uint32_t end[];
    extern uint32_t __StackLimit[];
    return uvisor_allocator_create_with_pool(end, (size_t)__StackLimit - (size_t)end);
}

UvisorAllocator uvisor_get_allocator(void)
{
    if (!malloc_allocator) uvisor_set_allocator(uvisor_allocator_init());
    return malloc_allocator;
}
int uvisor_set_allocator(UvisorAllocator allocator)
{
    if (allocator == NULL) return -1;
    DPRINTF("uvisor_allocator: Setting new allocator %p\n", allocator);
    malloc_allocator = allocator;
    return 0;
}

void *__real__malloc_r(struct _reent*, size_t);
void *__wrap__malloc_r(struct _reent*r, size_t size) {
    static unsigned int count = 0;
    (void)r;
    count++;
    DPRINTF(">> malloc #%u: %uB for %p\n", count, size, malloc_allocator);
    if (!malloc_allocator) uvisor_set_allocator(uvisor_allocator_init());
    return uvisor_malloc(malloc_allocator, size);
}

void *__real__realloc_r(struct _reent*, void *, size_t);
void *__wrap__realloc_r(struct _reent*r, void *ptr, size_t size) {
    static unsigned int count = 0;
    (void)r;
    count++;
    DPRINTF(">> realloc #%u: %u at %p for %p\n", count, size, ptr, malloc_allocator);
    if (!malloc_allocator) uvisor_set_allocator(uvisor_allocator_init());
    return uvisor_realloc(malloc_allocator, ptr, size);
}

void __real__free_r(struct _reent*, void *);
void __wrap__free_r(struct _reent*r, void *ptr) {
    static unsigned int count = 0;
    (void)r;
    count++;
    DPRINTF(">> free #%u: %p for %p\n", count, ptr, malloc_allocator);
    if (!malloc_allocator) uvisor_set_allocator(uvisor_allocator_init());
    uvisor_free(malloc_allocator, ptr);
}
