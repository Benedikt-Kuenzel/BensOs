#include "kernel/sd_card_driver.h"
#include "kernel/kerio.h"
#include "kernel/irqHandlers.h"


void sd_card_irq_handler(void){
    puts("SD IRQ HANDLER");
}

void sd_card_irq_clearer(void){}

void sd_driver_init(void){

    puts("sd_init");
    register_handler(2, &sd_card_irq_handler, &sd_card_irq_clearer);
}
void sd_send_read_request(uint32_t data_address){

    cmdtm_entry entry;
    entry.tm_blkcnt_en = 0;
    entry.tm_auto_cmd_en = 0b00;
    entry.tm_dat_dir = 1;
    entry.tm_multi_block = 0;
    entry.cmd_rspns_type = 0;
    entry.cmd_crcchk_en = 0;
    entry.cmd_ixchk_en = 0;
    entry.cmd_isdata = 1;
    entry.cmd_type = 0;
    entry.cmd_index = 17;

    cmdtm_entry * entryreg = 0x7E300000 + 0xc;

    *entryreg = entry;
   
    uint32_t * argumentreg = 0x7E300000 + 0x8; 

    *argumentreg = data_address;

    puts("sd_send");
}
