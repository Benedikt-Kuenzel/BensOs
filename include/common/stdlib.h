#ifndef STDLIB_H_   /* Include guard */
#define STDLIB_H_
#include <stddef.h>
#include <stdint.h>
typedef enum {
    NONE = 0x00000000,
    CORE = 0x54410001,
    MEM = 0x54410002
} atag_tag_t;

typedef struct {
    uint32_t size;
    uint32_t start;
} mem_t;

typedef struct atag {
    uint32_t tag_size;
    atag_tag_t tag;
    union {
        mem_t mem;
    };
} atag_t;

uint32_t get_mem_size(atag_t * tag);
uint32_t integer_to_string(char *buf, size_t bufsize, int n);
void bzero(void * dest, int bytes);
char * itoa(int num, int base);
int atoi(char * num);
void memcpy(void * dest, const void * src, int bytes);
#endif