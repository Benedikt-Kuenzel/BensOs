
.section ".lowtext.boot"

.global _start


_start:
    mrc p15, #0, r1, c0, c0, #5
    and r1, r1, #3
    cmp r1, #0
    bne halt

    mov sp, #0x8000

    ldr r4, =__bsss_start
    ldr r9, =__bsss_end
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    b       2f

1:
    stmia r4!, {r5-r8}

2:
    
    cmp r4, r9
    blo 1b

    ldr r3, = early_init
    blx r3

    ldr r3, = kernel_main
    blx r3

halt:
    wfe
    b halt