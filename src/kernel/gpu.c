#include <kernel/framebuffer.h>
#include <kernel/gpu.h>
#include <kernel/mem.h>
#include <kernel/kerio.h>
#include <kernel/mailbox.h>
#include <kernel/chars_pixels.h>
#include <common/stdlib.h>
#include <kernel/uart.h>




void write_pixel(uint32_t x, uint32_t y, const pixel_t * pix) {
    uint16_t * location = fbinfo.buf + y*fbinfo.pitch + x*BYTES_PER_PIXEL;
    memcpy(location, pix, BYTES_PER_PIXEL);
}

void gpu_putc(char c) {
    static const pixel_t WHITE = {0xff, 0xff, 0xff};
    static const pixel_t BLACK = {0x00, 0x00, 0x00};
    uint8_t w,h;
    uint8_t mask;
    const uint8_t * bmp = font(c);
    uint32_t i, num_rows = fbinfo.height/CHAR_HEIGHT;

    // shift everything up one row
    if (fbinfo.chars_y >= num_rows) {
        // Copy a whole character row into the one above it
        for (i = 0; i < num_rows-1; i++){
            memcpy(fbinfo.buf + fbinfo.pitch*i*CHAR_HEIGHT, fbinfo.buf + fbinfo.pitch*(i+1)*CHAR_HEIGHT, fbinfo.pitch * CHAR_HEIGHT);
        }
           
        // zero out the last row
        bzero(fbinfo.buf + fbinfo.pitch*i*CHAR_HEIGHT,fbinfo.pitch * CHAR_HEIGHT);
        fbinfo.chars_y--;
    }

    if (c == '\n') {
        fbinfo.chars_x = 0;
        fbinfo.chars_y++;
        return;
    }

    for(w = 0; w < CHAR_WIDTH; w++) {
        for(h = 0; h < CHAR_HEIGHT; h++) {
            mask = 1 << (w);
            if (bmp[h] & mask)
                write_pixel(fbinfo.chars_x*CHAR_WIDTH + w, fbinfo.chars_y*CHAR_HEIGHT + h, &WHITE);
            else
                write_pixel(fbinfo.chars_x*CHAR_WIDTH + w, fbinfo.chars_y*CHAR_HEIGHT + h, &BLACK);
        }
    }

    fbinfo.chars_x++;
    if (fbinfo.chars_x > fbinfo.chars_width) {
        fbinfo.chars_x = 0;
        fbinfo.chars_y++;
    }
}

void gpu_puts(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; i ++)
        gpu_putc((unsigned char)str[i]);
}

void gpu_init(void) {
    static const pixel_t BLACK = {0x00, 0x00, 0x00};
    // Aparantly, this sometimes does not work, so try in a loop
    //while(framebuffer_init());
       property_message_tag_t tags[5];

    uart_puts("Initializing Framebuf\n");
    tags[0].proptag = FB_SET_PHYSICAL_DIMENSIONS;
    tags[0].value_buffer.fb_screen_size.width = 640;
    tags[0].value_buffer.fb_screen_size.height = 480;
    tags[1].proptag = FB_SET_VIRTUAL_DIMENSIONS;
    tags[1].value_buffer.fb_screen_size.width = 640;
    tags[1].value_buffer.fb_screen_size.height = 480;
    tags[2].proptag = FB_SET_BITS_PER_PIXEL;
    tags[2].value_buffer.fb_bits_per_pixel = COLORDEPTH;
    tags[3].proptag = NULL_TAG;


    // Send over the initialization
    if (send_messages(tags) != 0) {
        uart_puts("Framebuf 0 \n");
        return -1;
    }

   
    fbinfo.width = tags[0].value_buffer.fb_screen_size.width;
    fbinfo.height = tags[0].value_buffer.fb_screen_size.height;
    fbinfo.chars_width = fbinfo.width / CHAR_WIDTH;
    fbinfo.chars_height = fbinfo.height / CHAR_HEIGHT;
    fbinfo.chars_x = 0;
    fbinfo.chars_y = 0;
    fbinfo.pitch = fbinfo.width*BYTES_PER_PIXEL;

    // request a framebuffer
    tags[0].proptag = FB_ALLOCATE_BUFFER;
    tags[0].value_buffer.fb_screen_size.width = 0;
    tags[0].value_buffer.fb_screen_size.height = 0;
    tags[0].value_buffer.fb_allocate_align = 16;
    tags[1].proptag = NULL_TAG;


    if (send_messages(tags) != 0) {
         uart_puts("Framebuf 0 \n");
        return -1;
    }

    fbinfo.buf = tags[0].value_buffer.fb_allocate_res.fb_addr;
    fbinfo.buf_size =  tags[0].value_buffer.fb_allocate_res.fb_size;
  
    uart_puts("Framebuf initialized\n");

    // clear screen
    for (uint32_t j = 0; j < fbinfo.height; j++) {
        for (uint32_t i = 0; i < fbinfo.width; i++) {
            write_pixel(i,j,&BLACK);
        }
    }
    
}
