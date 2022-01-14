#include "kernel/spinlock.h"

extern int try_lock(int * lock_var);


void spin_init(spin_lock_t * lock) {
    *lock = 1;
}

void spin_lock(spin_lock_t * lock) {
    while (!try_lock(lock));
}

void spin_unlock(spin_lock_t * lock) {
    *lock = 1;
}