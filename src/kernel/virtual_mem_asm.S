.section ".lowtext"

.global init_ttbcr
.global init_ttbr
.global enable_mmu
.global tlb_invalidate
.global disable_mmu

// void init_ttbcr(void)
init_ttbcr:
    MRC p15, 0, r0, c2, c0, 2					/* Read TTBCR into R0 */
	ORR r0, r0, #0b000								/* Set N = 1 */						
	MCR p15, 0, r0, c2, c0, 2	
    blx     lr 

//void init_ttbr(int32_t ttbr)
init_ttbr:
    MCR     p15, 0, r0, c2, c0, 0
    //MCR     p15, 0, r0, c2, c0, 1 
    blx lr

//void enable_mmu(int32_t ttbr0_entry, int32_t ttbr1_entry)
//this just flips bit [0] of the system control register (sctlr) to 1
//which enables the mmu
enable_mmu: 
    //init ttbcr, specifically ttbcr.n				/* Read TTBCR into R0 */
    mov r2, #0b001								/* Set N = 1 */						
    MCR p15, 0, r2, c2, c0, 2

    /* Set the access control to all-supervisor */
    //mov r0, #~0
    //mcr p15, 0, r0, c3, c0, 0

    mov r2,#0
    // invalidate caches
    mcr p15,0,r2,c7,c7,0 
    // invalidate TLB entries
    mcr p15,0,r2,c8,c7,0 
    // data synchronisation barrier
    mcr p15,0,r2,c7,c10,4 

    // set all domains to 0b11
    ldr r2, =0xffffffff
    mcr p15,0,r2,c3,c0,0

    
    //write the base adress of translation table 0
    //into ttbcr0
    MCR     p15, 0, r0, c2, c0, 0
    isb
    
    //write the base adress of translation table 1
    //into ttbcr1
    MCR     p15, 0, r1, c2, c0, 1
    isb

    //enable paging
    mrc p15, 0, r12, c1, c0, 0
    orr r12, r12, #0x1
    mcr p15, 0, r12, c1, c0, 0
    isb
    
    blx  lr


tlb_invalidate:
    mov r2,#0
    // invalidate TLB entries
    mcr p15,0,r1,c8,c7,0 
    // data synchronisation barrier
    mcr p15,0,r1,c7,c10,4 
    mov pc,lr


disable_mmu:
    mrc p15, 0, r12, c1, c0, 0
    and r12, r12, #0x0
    mcr p15, 0, r12, c1, c0, 0

    blx lr