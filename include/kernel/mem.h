#ifndef MEM_H_   /* Include guard */
#define MEM_H_
#define PAGE_SIZE 4096
#define KERNEL_HEAP_SIZE (1024*1024)
#include <kernel/list.h>
#include "common/stdlib.h"

void mem_init(atag_t * atags);
void * alloc_page(void);
void free_page(void * ptr);
void * kmalloc(uint32_t bytes);
void kfree(void *ptr);

#endif