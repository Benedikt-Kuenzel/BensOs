
#include <stdint.h>
#include <common/stdlib.h>
#include <kernel/list.h>
#ifndef MEM_H
#define MEM_H

#define PAGE_SIZE 4096
#define KERNEL_HEAP_SIZE (1024*1024)
#define KERNEL_STACK_SIZE PAGE_SIZE  

typedef struct {
	uint8_t allocated: 1;			// This page is allocated to something
	uint8_t kernel_page: 1;			// This page is a part of the kernel
	uint8_t kernel_heap_page: 1;	// This page is a part of the kernel
	uint32_t reserved: 29;
} page_flags_t;

DEFINE_LIST(page);

typedef struct page {
	uint32_t vaddr_mapped;	// The virtual address that maps to this page	
	page_flags_t flags;
	DEFINE_LINK(page);
} page_t;


typedef struct __attribute__((packed)) 
{
    uint32_t cirgn: 1; //cachable bit
    uint32_t s :1; //sharable bit
    uint32_t r1 : 1; //reserved
    uint32_t rgn: 2; //region bits
    uint32_t nos : 1; // not outer sharable bit
    uint32_t irgn: 1; // inner region bit 0
    uint32_t r2 : 5; //reserved 
    uint32_t baddr :20 ;//translation table base adress
}    ttbr0_entry_t;

//this must be 2^12 byte aligned
typedef struct __attribute__((packed))  {
    uint32_t bit0 : 1;
    uint32_t bit1 : 1;
    uint32_t pxn : 1;
    uint32_t ns: 1;
    uint32_t sbz: 1;
    uint32_t domain : 4;
    uint32_t imp : 1;
    uint32_t baddr : 22; //points to page table.
} page_directory_entry_t;

//this must be 2^10 byte aligned
typedef struct __attribute__((packed)) {
    uint32_t xn : 1;
    uint32_t bit1 : 1;
    uint32_t b : 1;
    uint32_t c: 1;
    uint32_t ap : 2;
    uint32_t tex: 3;
    uint32_t ap2: 1;
    uint32_t s : 1;
    uint32_t ng : 1;
    uint32_t baddr: 20; //points to actual page base addr. 
} page_table_entry_t;

int32_t identy_pagedir_page_addr;
int32_t identy_pagetable_page_addr;

void mem_init(atag_t * atags);

void * alloc_page(void);
void free_page(void * ptr);

void * kmalloc(uint32_t bytes);
void kfree(void *ptr);

void paging_enable_with_identitymapping(void);

#endif