#include "kernel/irqHandlers.h"


void register_handler(int32_t id, int32_t handler, int32_t clearer){
    irq_handlers handl;

    handl.handler = handler;
    handl.clearer = clearer;

    irqhandlers[id] = handl;
}