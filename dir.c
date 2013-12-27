#include <conio.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <cbm.h>

#include "dir.h"

void dir ()
{
    struct cbm_dirent dirent;

    cbm_opendir(8, 8);
    while (cbm_readdir(8, &dirent) == 0)
    {
        switch (dirent.type)
        {
        case CBM_T_HEADER:
            revers (true);
            printf("%s\r\n", dirent.name);
            revers (false);
            break;
        default:
            printf("%-16s %03d\r\n", dirent.name, dirent.size);
            break;
        }
    }
    cbm_closedir(8);
}
