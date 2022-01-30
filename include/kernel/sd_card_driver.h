#ifndef SD_CARD_DRIVER_H
#define SD_CARD_DRIVER_H

#include "stdint.h"
#include <kernel/list.h>
#include <stddef.h>


typedef struct __attribute__((packed)) {
    uint32_t sbz0 : 1;
    uint32_t tm_blkcnt_en: 1;
    uint32_t tm_auto_cmd_en : 2; 
    uint32_t tm_dat_dir :1; 
    uint32_t tm_multi_block: 1; 
    uint32_t sbz1 : 10; 
    uint32_t cmd_rspns_type:2; 
    uint32_t sbz2 : 1; 
    uint32_t cmd_crcchk_en : 1; 
    uint32_t cmd_ixchk_en : 1;
    uint32_t cmd_isdata :1;
    uint32_t cmd_type : 2;
    uint32_t cmd_index : 6;
    uint32_t sbz3: 2; 
} cmdtm_entry;

void sd_send_read_request(uint32_t);
void sd_card_iirq_handler(void);
void sd_card_iirq_clearer(void);
void sd_driver_init(void);
#endif