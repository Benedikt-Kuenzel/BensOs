
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#define COLORDEPTH 24
#define BYTES_PER_PIXEL COLORDEPTH/8
#include <stdint.h>
typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t vwidth;
    uint32_t vheight;
    uint32_t bytes;
    uint32_t depth;
    uint32_t ignorex;
    uint32_t ignorey;
    void * pointer;
    uint32_t size;
} fb_init_t;



typedef struct framebuffer_info {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    void * buf;
    uint32_t buf_size;
    uint32_t chars_width;
    uint32_t chars_height;
    uint32_t chars_x;
    uint32_t chars_y;
} framebuffer_info_t;
static fb_init_t fbinit __attribute__((aligned(16)));
static framebuffer_info_t fbinfo;
int framebuffer_init(void);

#endif