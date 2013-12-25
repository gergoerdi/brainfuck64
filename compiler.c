#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "compile.h"

/* conio doesn't echo typed characters.
** So, this function does it.
*/
static int gettoken(void) {
    int c;
    for (;;)
    {
        c = (int)cgetc();
        if (c == 0x03) // STOP
        {
            return EOF;
        }

        switch (c)
        {
        case '<':
        case '>':
        case '+':
        case '-':
        case '[':
        case ']':
        case '.':
            cputc(c);
            return c;
        case '\n':
            cputc(c);
            return c;
        }
    }
}

#define src_buffer_len 1024
char src_buffer[src_buffer_len];
int len = 0;

int main() {
    unsigned char height, width;
    unsigned char row = 7;
    unsigned char col = 0;
    int c;

    clrscr();
    cursor(true);

    screensize(&width, &height);

    /* This prompt fits on the VIC-20's narrow screen.
     */
    cprintf("Type your Brainfuck program below,\r\n"
            "finish with RUN/STOP:\r\n\n");

    while ((c = gettoken()) != EOF)
    {
        if (row == height)
        {
            clrscr();
            col = 0;
            cputc(c);	/* echo char. again because screen was erased */
            row = 1;
        }

        src_buffer[++len] = (char)c;
        if (len == src_buffer_len)
            break;

        if (++col == width)
        {
            /* cprintf("\r\n"); */
            ++row;
            col = 0;
        } else if (c == '\n')
        {
            cprintf("\r");
            ++row;
            col = 0;
        }
    }

    cprintf("\r\n");
    cprintf("Program length: %d\r\n", len);

    compile(src_buffer, len, 0x3000, 0xc000);

    // Clear STOP status
    *((char*)0x0091) = 0;

    return EXIT_SUCCESS;
}
