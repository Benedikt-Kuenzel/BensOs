#include "kernel/kerio.h"
#include "kernel/exceptionHandlers.h"
#include "kernel/timer.h"
#include "kernel/irqHandlers.h"




int IRQ_IS_PENDING(int32_t j){
    volatile int32_t irqAddr = 0x3F00B000;
    int32_t bit = j % 32;
    int32_t offset = 0;

    if(j >= 0)
        offset = 0x04;
    if(j > 31 )
        offset = 0x08;
    
    if(j > 63)
        offset = 0x00;
    
    if(j == 73){
        int32_t mask = 1 << (3);
        return ((*(int32_t *)0x40000060) & mask ) == mask;
    }
        

    if(j > 73 )
        return 0;

    if( 0 > j)
        return 0;

    
    int32_t mask = 1 << (bit);
    return ((*(int32_t *)irqAddr + offset) & mask ) == mask;
}

void irq_handler(void) {
    int j;
    for(j = 0; j < 74; j++){
        if (IRQ_IS_PENDING(j)  && (*irqhandlers[j].clearer != 0)) {
            (*irqhandlers[j].clearer)();
            DISABLE_INTERRUPTS();
            (* irqhandlers[j].handler)();
            ENABLE_INTERRUPTS();
        }
    }
}


void __attribute__ ((interrupt ("ABORT"))) reset_handler(void) {
    puts("RESET HANDLER\n");
    while(1);
}
void __attribute__ ((interrupt ("ABORT"))) prefetch_abort_handler(void) {
    puts("PREFETCH ABORT HANDLER\n");
    while(1);
}
void __attribute__ ((interrupt ("ABORT"))) data_abort_handler(void) {
    puts("DATA ABORT HANDLER\n");
    while(1);
}
void __attribute__ ((interrupt ("UNDEF"))) undefined_instruction_handler(void) {
    puts("UNDEFINED INSTRUCTION HANDLER\n");
    while(1);
}
void __attribute__ ((interrupt ("SWI"))) software_interrupt_handler(void) {
    printf("SWI HANDLER\n");
    while(1);
}
void __attribute__ ((interrupt ("FIQ"))) fast_irq_handler(void) {
    puts("FIQ HANDLER\n");
    while(1);
}


void initExceptions(void){
    move_exception_vector();
    ENABLE_INTERRUPTS();
}