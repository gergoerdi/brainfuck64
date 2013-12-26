#include <conio.h>
#include <stdlib.h>

#include "reader.h"
#include "compile.h"

int main()
{
    char c;
    const unsigned int max_len = 1024;
    const unsigned int prog_origin = 0xc000;
    const unsigned int mem_origin = 0x3000;
    unsigned int len = 0;

    compile_init (mem_origin, prog_origin);

    reader_init ();
    while (len < max_len && (c = reader_get ()))
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
