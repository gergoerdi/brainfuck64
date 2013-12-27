#include "reader.h"

#include <conio.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

char to_hex_digit (char x)
{
    return (x <= 9) ? '0' + x : 'a' + (x - 10);
}

char from_hex_digit (char c)
{
    return
        ('0' <= c && c <= '9') ? c - '0' :
        ('a' <= c && c <= 'f') ? c - 'a' + 10 :
        ('A' <= c && c <= 'F') ? c - 'A' + 10 :
        -1;
}

unsigned int read_hex (unsigned int initial)
{
    char initial_lo = *(char*)&initial;
    char initial_hi = *((char*)&initial + 1);
    char c, x;
    int pos = 0;


    char str[4];
    int col = wherex();
    int i;

    str[0] = initial_hi >> 4;
    str[1] = initial_hi & 0x0f;
    str[2] = initial_lo >> 4;
    str[3] = initial_lo & 0x0f;

    cursor(true);

    for (;;)
    {
        gotox (col);
        for (i = 0; i < 4; ++i)
            putchar (to_hex_digit (str[i]));
        gotox (col + pos);

        c = cgetc ();
        switch (c)
        {
        case CH_ENTER:
            puts ("");
            return str[0] * 16 * 16 * 16 + str[1] * 16 * 16 + str[2] * 16 + str[3];
            break;
        case CH_DEL:
            if (pos > 0) --pos;
            break;
        default:
            x = from_hex_digit (c);
            if (x != (char)-1)
            {
                str[pos] = x;
                if (pos < 3) ++pos;
            }
        }
    }
}

static char gettoken()
{
    char c;
    for (;;)
    {
        c = cgetc();

        switch (c)
        {
        case CH_STOP:
            return 0;
        case '<':
        case '>':
        case '+':
        case '-':
        case '[':
        case ']':
        case '.':
        case ' ':
        case '\n':
            cputc((int)c);
            return c;
        }
    }
}

static unsigned char height, width;
static unsigned char row;
static unsigned char col;

void reader_init ()
{
    screensize(&width, &height);
    col = wherex();
    row = wherey();

    cursor(true);
}

char reader_get ()
{
    char c = gettoken ();
    if (!c)
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
