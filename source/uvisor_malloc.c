
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <reent.h>

#include "uvisor-lib/uvisor-lib.h"

#define DPRINTF(...) {};
/* Use printf with caution inside malloc: printf may allocate memory, so
   using printf in malloc may lead to recusive calls! */
/* #define DPRINTF(fmt, ...) printf(fmt, ## __VA_ARGS__) */

extern UvisorBoxIndex *const __uvisor_ps;


int uvisor_set_allocator(UvisorAllocator allocator)
{
    DPRINTF("uvisor_allocator: Setting new allocator %p\n", allocator);

    if (allocator == NULL) return -1;
    __uvisor_ps->active_heap = allocator;
    return 0;
}

static inline int check_allocator()
{
    if (__uvisor_ps == NULL) return -1;

    if (__uvisor_ps->active_heap == NULL) {
        /* we need to initialize the process heap */
        if (__uvisor_ps->process_heap != NULL) {
            /* initialize the process heap */
            UvisorAllocator allocator = uvisor_allocator_create_with_pool(
                __uvisor_ps->process_heap,
                __uvisor_ps->process_heap_size);
            /* set the allocator */
            return uvisor_set_allocator(allocator);
        }
        DPRINTF("uvisor_allocator: No process heap available!\n");
        return -1;
    }
    return 0;
}

/* public API */
UvisorAllocator uvisor_get_allocator(void)
{
    if (check_allocator()) return NULL;
    return __uvisor_ps->active_heap;
}

/* wrapped memory management functions */
void *__real__malloc_r(struct _reent*, size_t);
void *__wrap__malloc_r(struct _reent*r, size_t size) {
    static unsigned int count = 0;
    (void)r;
    count++;
    DPRINTF(">> malloc #%u: %uB for %p\n", count, size, __uvisor_ps->active_heap);

    if (check_allocator()) return NULL;
    return uvisor_malloc(__uvisor_ps->active_heap, size);
}

void *__real__realloc_r(struct _reent*, void *, size_t);
void *__wrap__realloc_r(struct _reent*r, void *ptr, size_t size) {
    static unsigned int count = 0;
    (void)r;
    count++;
    DPRINTF(">> realloc #%u: %u at %p for %p\n", count, size, ptr, __uvisor_ps->active_heap);

    if (check_allocator()) return NULL;
    return uvisor_realloc(__uvisor_ps->active_heap, ptr, size);
}

void __real__free_r(struct _reent*, void *);
void __wrap__free_r(struct _reent*r, void *ptr) {
    static unsigned int count = 0;
    (void)r;
    count++;
    DPRINTF(">> free #%u: %p for %p\n", count, ptr, __uvisor_ps->active_heap);

    if (check_allocator()) return;
    uvisor_free(__uvisor_ps->active_heap, ptr);
}
