#include <stddef.h>
#include <stdint.h>
#include "kernel/uart.h"
#include "kernel/mem.h"
#include "common/stdlib.h"
#include "common/stdio.h"
#include <kernel/kerio.h>
#include <kernel/gpu.h>

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

    uart_puts("Booted.\n");
    uart_puts("Welcome to Ben's OS!\n");
    while (1) {
        uart_putc(uart_getc());
        uart_putc('\n');
    }
}
