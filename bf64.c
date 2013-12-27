#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <cbm.h>

#include "menu.h"
#include "reader.h"
#include "compile.h"

void bf_input()
{
    char c;
    unsigned int len = 0;

    clrscr();
    printf("Type your Brainfuck program below,\r\n"
           "finish with RUN/STOP:\r\n\n");

    reader_init ();
    while (c = reader_get ())
    {
        compile (c);
        ++len;
    }
    printf("\r\n");
    printf("Program length: %d\r\n", len);
}

void bf_file(const char *filename, bool show)
{
    int c;
    FILE *f;
    unsigned int len = 0;

    if (show)
        clrscr();

    printf("Loading from %s\r\n\r\n", filename);

    f = fopen (filename, "r");
    while ((c = fgetc(f)) != EOF)
    {
        if (show)
            putchar(c);
        compile (c);
        ++len;
    }
    fclose (f);

    if (show)
        printf("\r\n");

    printf("\r\nProgram source length: %d\r\n", len);
}

void bf_compile (const char *filename)
{
    unsigned int prog_origin;
    unsigned int mem_origin;

    printf ("Memory start address? ");
    mem_origin = read_hex (0x3000);

    printf ("Program start address? ");
    prog_origin = read_hex (0xc000);

    compile_init (mem_origin, prog_origin);
    if (filename)
        bf_file (filename, true);
    else
        bf_input ();
    compile_finish ();
    printf("Run with\r\nsys %u\r\n", prog_origin);
}

int main()
{
    clrscr();
    cprintf ("Brainfuck 64\r\n(C) 2013 Gergo Erdi\r\nhttp://gergo.erdi.hu/\r\n\r\n\r\n");

    bf_compile (main_menu ());

    // Clear STOP status
    *((char*)0x0091) = 0;
    return EXIT_SUCCESS;
}
