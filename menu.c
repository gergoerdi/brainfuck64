#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <cbm.h>

#include "menu.h"
#include "dir.h"

static char filename[17];

#define MENU(s) { revers(true); putchar(s[0]); revers(false); printf(s+1); }

char mode_menu ()
{
    bool valid = false;
    char c;
    cursor(true);

    MENU("Machine Code / ");
    MENU("BASIC ?");
    valid = false;

    for (;;)
    {
        c = toupper (cgetc ());
        switch (c)
        {
        case 'M':
        case 'B':
            printf ("%c\n\n", c);
            return c;
        }
    }
}

char * main_menu ()
{
    bool valid;

    cursor(true);
    for (;;)
    {
        MENU("Dir / ");
        MENU("Load / ");
        MENU("Input ? ");
        valid = false;

        while (!valid)
        {
            switch (toupper (cgetc ()))
            {
            case 'D':
                valid = true;
                printf ("D\r\n\r\n");
                dir ();
                printf ("\r\n");
                break;
            case 'L':
                puts ("L\r\n");
                printf ("Filename? ");
                gets(filename);
                puts ("\r\n");
                return filename;
            case 'I':
                printf ("I\r\n\r\n");
                return 0;
            }
        }
    }
}
