.section ".text"

.global init_ttbcr
.global init_ttbr
.global enable_mmu
.global tlb_invalidate

// void init_ttbcr(void)
init_ttbcr:
    MRC p15, 0, r0, c2, c0, 2					/* Read TTBCR into R0 */
	ORR r0, r0, #0b010								/* Set N = 1 */						
	MCR p15, 0, r0, c2, c0, 2	
    blx     lr 

//void init_ttbr(int32_t ttbr)
init_ttbr:
    MCR     p15, 0, r0, c2, c0, 0
    MCR     p15, 0, r0, c2, c0, 1 
    blx lr

//void enable_mmu(void)
//this just flips bit [0] of the system control register (sctlr) to 1
//which enables the mmu
enable_mmu:
     mov r1,#0
    // invalidate caches
    mcr p15,0,r1,c7,c7,0 
    // invalidate TLB entries
    mcr p15,0,r1,c8,c7,0 
    // data synchronisation barrier
    mcr p15,0,r1,c7,c10,4 

    // set all domains to 0b11
    //ldr r1, =0xffffffff
    //mcr p15,0,r1,c3,c0,0

    ldr r1, =0x00401805
    mrc p15,0,r2,c1,c0,0
    orr r2,r2,r1
    mcr p15,0,r2,c1,c0,0

    //mrc p15, 0, r12, c1, c0, 0
    //orr r12, r12, #0x1
    //mcr p15, 0, r12, c1, c0, 0
    blx lr


tlb_invalidate:
    mov r2,#0
    // invalidate TLB entries
    mcr p15,0,r1,c8,c7,0 
    // data synchronisation barrier
    mcr p15,0,r1,c7,c10,4 
    mov pc,lr