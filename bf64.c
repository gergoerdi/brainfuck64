#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <cbm.h>

#include "menu.h"
#include "reader.h"
#include "compile.h"
#include "transpile.h"

static void bf_input(bool transpile_mode)
{
    char c;
    unsigned int len = 0;

    clrscr();
    printf("Type your Brainfuck program below,\r\n"
           "finish with RUN/STOP:\r\n\n");

    reader_init ();
    while (c = reader_get ())
    {
        transpile_mode ? transpile (c) : compile (c);
        ++len;
    }
    printf("\r\n");
    printf("Program length: %d\r\n", len);
}

static void bf_file(bool transpile_mode, const char *filename, bool show)
{
    int c;
    FILE *f;
    unsigned int len = 0;

    if (show)
        clrscr();

    printf("Loading from %s\n\n", filename);

    f = fopen (filename, "r");
    while ((c = fgetc(f)) != EOF)
    {
        if (show)
            putchar(c);
        transpile_mode ? transpile (c) : compile(c);
        ++len;
    }
    fclose (f);

    if (show)
        printf("\n");

    printf("\nProgram source length: %d\n", len);
}

static void bf_compile (const char *filename)
{
    unsigned int prog_origin;
    unsigned int mem_origin;

    printf ("Memory start address? ");
    mem_origin = read_hex (0x3000);

    printf ("Program start address? ");
    prog_origin = read_hex (0x5000);

    compile_init (mem_origin, prog_origin);
    if (filename)
        bf_file (false, filename, true);
    else
        bf_input (false);
    compile_finish ();
    printf("Run with\nsys %u\n", prog_origin);
}

static void bf_transpile (const char *filename)
{
    unsigned int prog_origin, prog_end;

    printf ("Program start address? ");
    prog_origin = read_hex (0xc000);

    transpile_init (prog_origin);
    if (filename)
        bf_file (true, filename, true);
    else
        bf_input (true);
    prog_end = transpile_finish ();

    *((unsigned int*)0x002b) = prog_origin+1;
    *((unsigned int*)0x002d) = prog_end+1;
    *((unsigned int*)0x002f) = prog_end+1;
    *((unsigned int*)0x0031) = prog_end+1;
    *((unsigned int*)0x0037) = prog_origin + 0x1000;
}

int main()
{
    char *filename;
    bool transpile;

    clrscr();
    printf ("Brainfuck 64\n(C) 2013 Gergo Erdi\nhttp://gergo.erdi.hu/\n\n\n");

    filename = main_menu ();
    transpile = mode_menu () == 'B';
    transpile ? bf_transpile(filename) : bf_compile(filename);

    // Clear STOP status
    *((char*)0x0091) = 0;
    return EXIT_SUCCESS;
}
