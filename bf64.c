#include <conio.h>
#include <stdlib.h>

#include "reader.h"
#include "compile.h"

#define src_buffer_len 1024
static char src_buffer[src_buffer_len];

#define mem 0x3000
#define target 0xc000

int main()
{
    char c;
    int len;

    reader_init ();
    while (len < src_buffer_len && (c = reader_get ()))
        src_buffer[len++] = c;
    cprintf("\r\n");
    cprintf("Program length: %d\r\n", len);

    compile_init (mem, target);
    compile(src_buffer, len);
    compile_finish ();

    cprintf("Run with\r\nsys %u\r\n", target);

    // Clear STOP status
    *((char*)0x0091) = 0;

    return EXIT_SUCCESS;
}
