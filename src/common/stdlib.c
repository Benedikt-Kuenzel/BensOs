#include "common/stdlib.h"

typedef struct{
    uint32_t div;
    uint32_t mod;
} divmod_t;


__inline__ uint32_t div(uint32_t dividend, uint32_t divisor) {
    return dividend / divisor;
}
__inline__ divmod_t divmod(uint32_t dividend, uint32_t divisor) {
    divmod_t res;
    res.div = dividend/divisor;
    res.mod = dividend % divisor;
    return res;
}


uint32_t get_mem_size(atag_t * tag) {
(void) tag;
return 1024 * 1024  * 1024;
//this is fine if atags are passed by bootloader, whic apperently neither 
//QEMU nor some acutal raspis do
//so we just return 1gb  memsize manually.
//  while (tag->tag != NONE) {
//      if (tag->tag == MEM) {
//          return tag->mem.size;
//      }
//      tag = ((uint32_t *)tag) + tag->tag_size;
//  }
//  return 0;
//
}

uint32_t integer_to_string(char *buf, size_t bufsize, int n)
{
   char *start;

   // Handle negative numbers.
   //
   if (n < 0)
   {
      if (!bufsize)
         return -1;

      *buf++ = '-';
      bufsize--;
   }

   // Remember the start of the string...  This will come into play
   // at the end.
   //
   start = buf;

   do
   {
      // Handle the current digit.
      //
      int digit;
      if (!bufsize)
         return -1;
      digit = n % 10;
      if (digit < 0)
         digit *= -1;
      *buf++ = digit + '0';
      bufsize--;
      n /= 10;
   } while (n);

   // Terminate the string.
   //
   if (!bufsize)
      return -1;
   *buf = 0;

   // We wrote the string backwards, i.e. with least significant digits first.
   // Now reverse the string.
   //
   --buf;
   while (start < buf)
   {
      char a = *start;
      *start = *buf;
      *buf = a;
      ++start;
      --buf;
   }

   return 0;
}

void bzero(void * dest, int bytes) {
    char * d = dest;
    while (bytes--) {
        *d++ = 0;
    }
}

void memcpy(void * dest, const void * src, int bytes) {
    char * d = dest;
    const char * s = src;
    while (bytes--) {
        *d++ = *s++;
    }
}

char * itoa(int num, int base) {
    static char intbuf[32];
    uint32_t j = 0, isneg = 0, i;
    divmod_t divmod_res;

    if (num == 0) {
        intbuf[0] = '0';
        intbuf[1] = '\0';
        return intbuf;
    }

    if (base == 10 && num < 0) {
        isneg = 1;
        num = -num;
    }

    i = (uint32_t) num;

    while (i != 0) {
       divmod_res = divmod(i,base);
       intbuf[j++] = (divmod_res.mod) < 10 ? '0' + (divmod_res.mod) : 'a' + (divmod_res.mod) - 10;
       i = divmod_res.div;
    }

    if (isneg)
        intbuf[j++] = '-';

    if (base == 16) {
        intbuf[j++] = 'x';
        intbuf[j++] = '0';
    } else if(base == 8) {
        intbuf[j++] = '0';
    } else if(base == 2) {
        intbuf[j++] = 'b';
        intbuf[j++] = '0';
    }

    intbuf[j] = '\0';
    j--;
    i = 0;
    while (i < j) {
        isneg = intbuf[i];
        intbuf[i] = intbuf[j];
        intbuf[j] = isneg;
        i++;
        j--;
    }

    return intbuf;
}

int atoi(char * num) {
    int res = 0, power = 0, digit, i;
    char * start = num;

    // Find the end
    while (*num >= '0' && *num <= '9') {
        num++;     
    }

    num--;

    while (num != start) {
        digit = *num - '0'; 
        for (i = 0; i < power; i++) {
            digit *= 10;
        }
        res += digit;
        power++;
        num--;
    }

    return res;
}
