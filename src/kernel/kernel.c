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


void test(void) {
    int i = 0;
    while (1) {
        puts("test \n");
        udelay(1000000);
    }
}


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


    paging_enable_with_identitymapping();

    
    create_kernel_thread(test, "TEST", 4);


    while (1) {
        puts("main \n");
        udelay(1000000);
    }
}
