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

int read_source (char *src_buffer, int max_len)
{
    unsigned char height, width;
    unsigned char row = 7;
    unsigned char col = 0;
    int c;
    int len = 0;

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

        src_buffer[len++] = (char)c;
        if (len == max_len)
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

    return len;
}
