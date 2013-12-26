#include "reader.h"

#include <conio.h>
#include <stdio.h>
#include <stdbool.h>

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

static unsigned char height, width;
static unsigned char row = 4;
static unsigned char col = 0;

void reader_init ()
{
    clrscr();
    cursor(true);
    screensize(&width, &height);

    cprintf("Type your Brainfuck program below,\r\n"
            "finish with RUN/STOP:\r\n\n");
}

char reader_get ()
{
    int c = gettoken ();
    if (c == EOF)
        return 0;

    if (row == height)
    {
        clrscr();
        col = 0;
        cputc(c);	/* echo char. again because screen was erased */
        row = 1;
    }

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

    return (char)c;
}
