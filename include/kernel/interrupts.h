#include <kernel/peripheral.h>
#include <stdint.h>
#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define INTERRUPTS_BASE (PERIPHERAL_BASE + INTERRUPTS_OFFSET)
#define INTERRUPTS_PENDING (INTERRUPTS_BASE + 0x200)

#endif