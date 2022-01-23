#include <kernel/mem.h>
#include <common/stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "kernel/uart.h"




/**
 * Heap Stuff
 */
static void heap_init(uint32_t heap_start);
/**
 * impliment kmalloc as a linked list of allocated segments.
 * Segments should be 4 byte aligned.
 * Use best fit algorithm to find an allocation
 */
typedef struct heap_segment{
    struct heap_segment * next;
    struct heap_segment * prev;
    uint32_t is_allocated;
    uint32_t segment_size;  // Includes this header
} heap_segment_t;

static heap_segment_t * heap_segment_list_head;

/**
 * End Heap Stuff
 */


extern uint8_t __end;
static uint32_t num_pages;

IMPLEMENT_LIST(page);

static page_t * all_pages_array;
page_list_t free_pages;



void mem_init(atag_t * atags) {
    uint32_t mem_size, page_array_len, kernel_pages, page_array_end, i;

    // Get the total number of pages
    mem_size = get_mem_size(atags);
    num_pages = mem_size / PAGE_SIZE;

    // Allocate space for all those pages' metadata.  Start this block just after the stack
    page_array_len = sizeof(page_t) * num_pages;
    all_pages_array = (page_t *)((uint32_t)&__end + KERNEL_STACK_SIZE);
    bzero(all_pages_array, page_array_len);
    INITIALIZE_LIST(free_pages);
    
    // Find where the page metadata ends and round up to the nearest page
    page_array_end = (uint32_t)all_pages_array + page_array_len;
    page_array_end += page_array_end % PAGE_SIZE ? PAGE_SIZE - (page_array_end % PAGE_SIZE) : 0;

    // Iterate over all pages and mark them with the appropriate flags
    // Start with kernel pages, stacks, and page metadata
    kernel_pages = (page_array_end) / PAGE_SIZE;
    for (i = 0; i < kernel_pages; i++) {
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE;    // Identity map the kernel pages
        all_pages_array[i].flags.allocated = 1;
        all_pages_array[i].flags.kernel_page = 1;
    }
     uart_puts(itoa(i,10));
     uart_puts("\n");
    // Reserve 1 MB for the kernel heap
    for(; i < kernel_pages + (KERNEL_HEAP_SIZE / PAGE_SIZE); i++){
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE;    // Identity map the kernel pages
        all_pages_array[i].flags.allocated = 1;
        all_pages_array[i].flags.kernel_heap_page = 1;
    }
    uart_puts(itoa(i,10));
    uart_puts("\n");
    // Map the rest of the pages as unallocated, and add them to the free list
    for(; i < num_pages; i++){
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE;    // Identity map the kernel pages
        all_pages_array[i].flags.allocated = 0;
        append_page_list(&free_pages, &all_pages_array[i]);
    }
uart_puts(itoa(i,10));
uart_puts("\n");

    // Initialize the heap
    heap_init(page_array_end);

}

void * alloc_page(void) {
    page_t * page;
    void * page_mem;

    if (size_page_list(&free_pages) == 0)
        return 0;

    // Get a free page
    page = pop_page_list(&free_pages);
    page->flags.kernel_page = 1;
    page->flags.allocated = 1;

    // Get the address the physical page metadata refers to
    page_mem = (void *)((page - all_pages_array) * PAGE_SIZE);

    // Zero out the page, big security flaw to not do this :)
    bzero(page_mem, PAGE_SIZE);

    return page_mem;
}

void free_page(void * ptr) {
    page_t * page;

    // Get page metadata from the physical address
    page = all_pages_array + ((uint32_t)ptr / PAGE_SIZE);

    // Mark the page as free
    page->flags.allocated = 0;
    append_page_list(&free_pages, page);
}


static void heap_init(uint32_t heap_start) {
   heap_segment_list_head = (heap_segment_t *) heap_start;
   bzero(heap_segment_list_head, sizeof(heap_segment_t));
   heap_segment_list_head->segment_size = KERNEL_HEAP_SIZE;
}


void * kmalloc(uint32_t bytes) {
    heap_segment_t * curr, *best = NULL;
    int diff, best_diff = 0x7fffffff; // Max signed int

    // Add the header to the number of bytes we need and make the size 16 byte aligned
    bytes += sizeof(heap_segment_t);
    bytes += bytes % 16 ? 16 - (bytes % 16) : 0;

    // Find the allocation that is closest in size to this request
    for (curr = heap_segment_list_head; curr != NULL; curr = curr->next) {
        diff = curr->segment_size - bytes;

 
        if (!curr->is_allocated && diff < best_diff && diff >= 0) {
            best = curr;
            best_diff = diff;
        }
    }
    

    // There must be no free memory right now :(
    if (best == NULL){
        uart_puts("No free memory!");
        return NULL;
    }

    // If the best difference we could come up with was large, split up this segment into two.
    // Since our segment headers are rather large, the criterion for splitting the segment is that
    // when split, the segment not being requested should be twice a header size
    if (best_diff > (int)(2 * sizeof(heap_segment_t))) {
        bzero(((void*)(best)) + bytes, sizeof(heap_segment_t));
        curr = best->next;
        best->next = ((void*)(best)) + bytes;
        best->next->next = curr;
        best->next->prev = best;
        best->next->segment_size = best->segment_size - bytes;
        best->segment_size = bytes;
    }

    best->is_allocated = 1;

    return best->next + 1;
}

void kfree(void *ptr) {
    heap_segment_t * seg;

    if (!ptr)
        return;

    seg = ptr - sizeof(heap_segment_t);
    seg->is_allocated = 0;

    // try to coalesce segements to the left
    while(seg->prev != NULL && !seg->prev->is_allocated) {
        seg->prev->next = seg->next;
        seg->next->prev = seg->prev;
        seg->prev->segment_size += seg->segment_size;
        seg = seg->prev;
    }
    // try to coalesce segments to the right
    while(seg->next != NULL && !seg->next->is_allocated) {
        seg->next->next->prev = seg;
        seg->next = seg->next->next;
        seg->segment_size += seg->next->segment_size;
        seg = seg->next;
    }
}


/*

PAGING

*/

extern void enable_mmu(uint32_t ttbr0_entry);
extern void disable_mmu(void);
extern void init_ttbr(int  ttbr);
extern void init_ttbcr(void);


void paging_enable_with_identitymapping(void){

    identy_pagedir_page_addr = 0x10004000;
    identy_pagetable_page_addr;

    page_directory_entry_t  page_dir_entry;
    page_table_entry_t table_entry;
   
    for(uint32_t i = 0;  4096 > i; i++){
        if(i % 4 == 0)
            identy_pagetable_page_addr = alloc_page();

        
        
        page_dir_entry.bit0 = 1;
        page_dir_entry.bit1 = 0;
        page_dir_entry.pxn = 0;
        page_dir_entry.ns = 0;
        page_dir_entry.sbz = 0;
        page_dir_entry.domain = 0xF; //manager access
        page_dir_entry.imp = 0;
        page_dir_entry.baddr = (identy_pagetable_page_addr + ((i % 4) * 1024)) >> 10; //align to 10bits
      

        uint32_t * dest = identy_pagedir_page_addr + (i * sizeof(page_directory_entry_t ));
        uint32_t * src = &page_dir_entry;
        

        *dest = *src;
        for(uint32_t k = 0; 256 > k; k++){

            table_entry.xn = 0;
            table_entry.bit1 = 1;
            table_entry.b = 0;
            table_entry.c = 0;
            table_entry.ap = 0b11;
            table_entry.tex = 0b000;
            table_entry.ap2 = 1;
            table_entry.s = 0;
            table_entry.ng = 0;
            table_entry.baddr = ((i*256) + k) ;

            uint32_t * dest2 = (identy_pagetable_page_addr + ((i % 4) * 1024)) + (k  * sizeof(page_table_entry_t));
            uint32_t * src2 = &table_entry;
            
                
            *dest2 = *src2;
        }
  
    }

    ttbr0_entry_t  ttbr_entry;

    ttbr_entry.nos = 0;
    ttbr_entry.rgn = 0b00;
    ttbr_entry.r1 = 0;
    ttbr_entry.s = 0;
    ttbr_entry.cirgn  = 0;
    ttbr_entry.r2 = 0;
    ttbr_entry.baddr = identy_pagedir_page_addr >> 14;
    
    uint32_t ttbr_int ;
    memcpy(&ttbr_int, &ttbr_entry, sizeof(ttbr0_entry_t));
    
    puts("Page directory created\n");
    puts("Page tables created\n");
    puts("Initializing MMU with identiy mapping...\n");
    enable_mmu(ttbr_int);
    puts("MMU initialized.\n");
}