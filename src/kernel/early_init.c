

#include <stddef.h>
#include <stdint.h>
#include "kernel/uart.h"

typedef struct __attribute__((packed)) 
{
    unsigned int cirgn: 1; //cachable bit
    unsigned int s :1; //sharable bit
    unsigned int r1 : 1; //reserved
    unsigned int rgn: 2; //region bits
    unsigned int nos : 1; // not outer sharable bit
    unsigned int irgn: 1; // inner region bit 0
    unsigned int r2 : 6; //sbz
    unsigned int baddr :19 ;//translation table base adress
}    ttbr0_entry_t;

//this must be 2^14 byte aligned
typedef struct __attribute__((packed))  {
    unsigned int bit0 : 1;
    unsigned int bit1 : 1;
    unsigned int pxn : 1;
    unsigned int ns: 1;
    unsigned int sbz: 1;
    unsigned int domain : 4;
    unsigned int imp : 1;
    unsigned int baddr : 22; //points to page table.
} page_directory_entry_t;

//this must be 2^10 byte aligned
typedef struct __attribute__((packed)) {
    unsigned int xn : 1;
    unsigned int bit1 : 1;
    unsigned int b : 1;
    unsigned int c: 1;
    unsigned int ap : 2;
    unsigned int tex: 3;
    unsigned int ap2: 1;
    unsigned int s : 1;
    unsigned int ng : 1;
    unsigned int baddr: 20; //points to actual page base addr. 
} page_table_entry_t;



__attribute__ ((section(".lowtext"))) extern void enable_mmu(unsigned int ttbr0_entry, unsigned int ttbr1_entry);


__attribute__ ((section(".lowtext"))) void early_init (void )
{
   
    unsigned int identy_pagedir_page_addr = 0x10004000;
    unsigned int identy_pagetable_page_addr = 0x10004000 + (4096 * 16);

    page_directory_entry_t  page_dir_entry;
    page_table_entry_t table_entry;
    unsigned int i;

    for(i = 0;  4096 /2> i; i++){
       
        if(i % 4 == 0)
            identy_pagetable_page_addr = identy_pagetable_page_addr + 4096;

        
        page_dir_entry.bit0 = 1;
        page_dir_entry.bit1 = 0;
        page_dir_entry.pxn = 0;
        page_dir_entry.ns = 0;
        page_dir_entry.sbz = 0;
        page_dir_entry.domain = 0xF; //manager access
        page_dir_entry.imp = 0;
        page_dir_entry.baddr = (identy_pagetable_page_addr + ((i % 4) * 1024)) >> 10; //align to 10bits
      

        unsigned int * dest = identy_pagedir_page_addr + (i * sizeof(page_directory_entry_t ));
        unsigned int  * src = &page_dir_entry;
        *dest = *src;

           for(unsigned int  k = 0; 256 > k; k++){

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

            unsigned int  * dest2 = (identy_pagetable_page_addr + ((i % 4) * 1024)) + (k  * sizeof(page_table_entry_t));
            unsigned int  * src2 = &table_entry;
            
                
            *dest2 = *src2;
        }
  
    }

   
    unsigned int  ttbr_int = identy_pagedir_page_addr;

    identy_pagedir_page_addr += (4096 * 4); //align to 16kb boundary for ttbr1
     
    for(; 4096> i; i++ ){
        if(i % 4 == 0)
            identy_pagetable_page_addr = identy_pagetable_page_addr + 4096;

        
        page_dir_entry.bit0 = 1;
        page_dir_entry.bit1 = 0;
        page_dir_entry.pxn = 0;
        page_dir_entry.ns = 0;
        page_dir_entry.sbz = 0;
        page_dir_entry.domain = 0xF; //manager access
        page_dir_entry.imp = 0;
        page_dir_entry.baddr = (identy_pagetable_page_addr + ((i % 4) * 1024)) >> 10; //align to 10bits
      

        unsigned int  * dest = identy_pagedir_page_addr + (i * sizeof(page_directory_entry_t ));
        unsigned int  * src = &page_dir_entry;
        

        *dest = *src;
     
            for(unsigned int  k = 0; 256 > k; k++){
            
                table_entry.xn = 0;
                table_entry.bit1 = 1;
                table_entry.b = 0;
                table_entry.c = 0;
                table_entry.ap = 0b11;
                table_entry.tex = 0b000;
                table_entry.ap2 = 1;
                table_entry.s = 0;
                table_entry.ng = 0;
                if((i * 256) + k  >= (0xC0000000 / 4096))
                    table_entry.baddr = ((i*256) + k) - 786432;
                else
                    table_entry.baddr = ((i*256) + k) ;

                unsigned int * dest2 = (identy_pagetable_page_addr + ((i % 4) * 1024)) + (k  * sizeof(page_table_entry_t));
                unsigned int * src2 = &table_entry;
                
                    
                *dest2 = *src2;
            }
    }
    
    unsigned int ttbr1_int  = identy_pagedir_page_addr;


    enable_mmu(ttbr_int, ttbr1_int);

}