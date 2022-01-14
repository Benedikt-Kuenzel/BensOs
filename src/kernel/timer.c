#include "kernel/timer.h"
#include "kernel/irqHandlers.h"
#include "kernel/kerio.h";
#include "kernel/process.h";

static timer_registers_t * timer_regs;

__attribute__ ((optimize(0))) void udelay (uint32_t usecs) {
    volatile uint32_t curr = timer_regs->counter_low;
    volatile uint32_t x = timer_regs->counter_low - curr;
    while (x < usecs) {
        x = timer_regs->counter_low - curr;
    }
}

void handle_timer_irq(void){
    disable_cntv();
    schedule();
}

void clear_timer_irq(void){
    return;
}
void timer_init(void) {
    timer_regs = (timer_registers_t *) 0x3F003000;
    register_handler(73, &handle_timer_irq, &clear_timer_irq);
}


#define CORE0_TIMER_IRQCNTL 0x40000040
#define CORE0_IRQ_SOURCE 0x40000060


static uint32_t cntfrq = 0;

void enable_cntv(void)
{
    uint32_t cntv_ctl;
    cntv_ctl = 1;
	asm volatile ("mcr p15, 0, %0, c14, c3, 1" :: "r"(cntv_ctl) ); // write CNTV_CTL
}

void disable_cntv(void)
{
    uint32_t cntv_ctl;
    cntv_ctl = 0;
	asm volatile ("mcr p15, 0, %0, c14, c3, 1" :: "r"(cntv_ctl) ); // write CNTV_CTL
}
void write_cntv_tval(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c14, c3, 0" :: "r"(val) );
    return;
}

uint32_t read_cntfrq(void)
{
    uint32_t val;
	asm volatile ("mrc p15, 0, %0, c14, c0, 0" : "=r"(val) );
    return val;
}

void routing_core0cntv_to_core0irq(void)
{
    mmio_write(CORE0_TIMER_IRQCNTL, 0x08);
}

void timer_set(uint32_t microseconds){
    cntfrq = read_cntfrq();
    write_cntv_tval(cntfrq / 1000000 * microseconds); 
    routing_core0cntv_to_core0irq();
    enable_cntv(); 
}