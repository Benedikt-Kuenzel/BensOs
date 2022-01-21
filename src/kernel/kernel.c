#include <stddef.h>
#include <stdint.h>
#include "kernel/uart.h"
#include "kernel/mem.h"
#include <kernel/kerio.h>
#include <kernel/gpu.h>
#include "common/stdlib.h"
#include "common/stdio.h"
#include "kernel/timer.h"
#include "kernel/process.h"

void hello(){
    puts("hello");
}
void test(void) {
    int i = 0;
    while (1) {
        puts("test \n");
        udelay(1000000);
    }
}

extern void init_ttbr(int  ttbr);
extern void init_ttbcr(void);


void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
    (void) r0;
    (void) r1;
    (void) atags;

    uart_init();
    uart_puts("Booting...\r\n");
    uart_puts("Initializing Memory Module\n");
    mem_init((atag_t *)atags);
    gpu_init();

   
    
    initExceptions();
    puts("Exception Vectors Initialized.\n");


    timer_init();
    puts("Timer initialized.\n");

    process_init();
    puts("Processes initialized.\n");

    puts("Booted.\n");
    puts("Welcome to Ben's OS!\n");

    create_kernel_thread(test, "TEST", 4);

    ttbr0_entry_t  ttbr_entry;
    ttbr_entry.baddr =0x2aaa;
    ttbr_entry.nos = 1;
    ttbr_entry.cirgn = 1;
    ttbr_entry.irgn = 1;
    ttbr_entry.rgn = 3;
    ttbr_entry.r2 = 127;
    int32_t ttbr_int;
    memcpy(&ttbr_int, &ttbr_entry, sizeof(ttbr0_entry_t));

   puts(itoa(ttbr_int, 2));
    puti(sizeof(ttbr0_entry_t));


    init_ttbcr();
    init_ttbr(ttbr_int);

    while (1) {
        puts("main \n");
        udelay(1000000);
    }
}
