#include <conio.h>
#include <stdlib.h>

#include "reader.h"
#include "compile.h"

int main()
{
    char c;
    unsigned int prog_origin;
    unsigned int mem_origin;
    unsigned int len = 0;

    cprintf ("Memory start address? ");
    mem_origin = read_hex (0x3000);

    cprintf ("Program start address? ");
    prog_origin = read_hex (0xc000);

    compile_init (mem_origin, prog_origin);

    clrscr();
    cprintf("Type your Brainfuck program below,\r\n"
            "finish with RUN/STOP:\r\n\n");

    reader_init ();
    while (c = reader_get ())
    {
        compile (c);
        ++len;
    }
    cprintf("\r\n");
    cprintf("Program length: %d\r\n", len);

    compile_finish ();

    cprintf("Run with\r\nsys %u\r\n", prog_origin);

    // Clear STOP status
    *((char*)0x0091) = 0;

    return EXIT_SUCCESS;
}
