ENTRY(_start)
 
SECTIONS
{
   /* Starts at LOADER_ADDR. */
    . = 0x8000;
    __start = .;
    .lowtext ALIGN (4K):
    {
       
        KEEP(*(.lowtext.boot))
        *(.lowtext)
    }
    __lowtext_end = .;

    __bsss_start = .;
    .bsss ALIGN (4K) :
    {
        bsss = .;
        *(.bsss)
    }
    __bsss_end = .;


    . += 0xC0000000;
    __text_start = .;
    .text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000)
    {
        *(.text)
    }
    __text_end = .;
 
    __rodata_start = .;
    .rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
    {
        *(.rodata)
    }
    __rodata_end = .;
 
    __data_start = .;
    .data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
    {
        *(.data)
    }
    __data_end = .;
 
    __bss_start = .;
    .bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000)
    {
        bss = .;
        *(.bss)
    }
    __bss_end = .;
    __end = .;

 
}
