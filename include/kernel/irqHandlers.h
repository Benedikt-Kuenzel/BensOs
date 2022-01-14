#include <stdint.h>
#ifndef IRQHANDLERS_H
#define IRQHANDLERS_H

#define NUM_IRQS 73;

typedef void (*func_pointer)(void);
typedef struct irq_handlers {
    func_pointer handler;
    func_pointer clearer;
} irq_handlers;

irq_handlers irqhandlers[73];

void register_handler(int32_t id, int32_t handler, int32_t clearer);

#endif
