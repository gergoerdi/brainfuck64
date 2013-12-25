#include <conio.h>
#include <stdlib.h>

#include "read.h"
#include "compile.h"

#define src_buffer_len 1024
static char src_buffer[src_buffer_len];

int main()
{
    int len;

    len = read_source(src_buffer, src_buffer_len);
    cprintf("\r\n");
    cprintf("Program length: %d\r\n", len);

    compile(src_buffer, len, 0x3000, 0xc000);

    // Clear STOP status
    *((char*)0x0091) = 0;

    return EXIT_SUCCESS;
}