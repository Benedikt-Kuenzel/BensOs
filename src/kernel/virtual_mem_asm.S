.section ".text"

.global init_ttbcr
.global init_ttbr

// void init_ttbcr(void)
init_ttbcr:
    mov     r1, #0x010 
    MCR     p15, 0, r1, c2, c0, 2
    blx     lr 

//void init_ttbr(int32_t ttbr)
init_ttbr:
    MCR p15, 0, r1, c2, c0, 0
    blx lr
