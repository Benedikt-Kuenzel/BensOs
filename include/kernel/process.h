#ifndef PROCESS_H
#define PROCESS_H

#include "stdint.h"
#include <kernel/list.h>
#include <stddef.h>

typedef struct {
    uint32_t r0;
    uint32_t r1; 
    uint32_t r2; 
    uint32_t r3; 
    uint32_t r4; 
    uint32_t r5; 
    uint32_t r6; 
    uint32_t r7; 
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t cpsr; 
    uint32_t sp;
    uint32_t lr;
} proc_saved_state_t;

typedef struct __attribute__((packed)) 
{
    uint32_t cirgn: 1; //cachable bit
    uint32_t s :1; //sharable bit
    uint32_t r1 : 1; //reserved
    uint32_t rgn: 2; //region bits
    uint32_t nos : 1; // not outer sharable bit
    uint32_t irgn: 1; // inner region bit 0
    uint32_t r2 : 7; //reserved 
    uint32_t baddr :18 ;//translation table base adress
}    ttbr0_entry_t;



typedef struct {
    uint32_t reserved: 30;
    uint8_t empty: 1;
    uint8_t full:1;
} page_directory_entry_t;

DEFINE_LIST(pcb);

typedef struct pcb {
    proc_saved_state_t * saved_state; // Pointer to where on the stack this process's state is saved. Becomes invalid once the process is running
    void * stack_page;                // The stack for this proces.  The stack starts at the end of this page
    uint32_t pid;                     // The process ID number
    DEFINE_LINK(pcb);
    char proc_name[20];               // The process's name
    ttbr0_entry_t* ttbr_entry;              //The ttbr so the tlb has the right adress to the page directory
} process_control_block_t;


typedef void (*main_func_pointer)(void);

void process_init(void);

void create_kernel_thread(main_func_pointer thread_func, char * name, int name_len);
void schedule(void);
#endif