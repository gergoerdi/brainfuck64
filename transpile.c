#include <stdlib.h>
#include <stdio.h>
#include <cbm.h>
#include <string.h>

#include "transpile.h"

static unsigned int pc;
static unsigned int *line_start;
static unsigned int basic_line;

#define emitB(b) { *((char*)pc++) = b; }
#define emitW(w) { *(((unsigned int*)pc)++) = w; }
#define emitS(s) { strcpy((char*)pc, s); pc += strlen(s); }
#define start_line() { line_start = (unsigned int*)pc; emitW(0); emitW(basic_line++)}
#define end_line() { emitB(0); *line_start = pc; }
#define rem(s) { emitS(" : "); emitB(0x8f); emitB(' '); emitS(s); }

static void transpile_bunch (char instr, char bunch_size)
{
    if (!bunch_size)
        return;

    switch (instr)
    {
    case '+':
        start_line();
        emitS("m%(p%)");
        emitB(0xb2); // =
        emitS("m%(p%)");
        emitB(0xaa); // +
        pc += sprintf((char*)pc, "%u", bunch_size);
        emitB(0xaf); // AND
        emitS("255");
        end_line();
        break;

    case '-':
        start_line();
        emitS("m%(p%)");
        emitB(0xb2); // =
        emitS("m%(p%)");
        emitB(0xab); // -
        pc += sprintf((char*)pc, "%u", bunch_size);
        emitB(0xaf); // AND
        emitS("255");
        end_line();
        break;

    case '>':
        start_line();
        emitS("p%");
        emitB(0xb2); // =
        emitS("p%");
        emitB(0xaa); // +
        pc += sprintf((char*)pc, "%u", bunch_size);
        emitB(0xaf); // AND
        emitS("255");
        end_line();
        break;

    case '<':
        start_line();
        emitS("p%");
        emitB(0xb2); // =
        emitS("p%");
        emitB(0xab); // -
        pc += sprintf((char*)pc, "%u", bunch_size);
        emitB(0xaf); // AND
        emitS("255");
        end_line();
        break;

    case '.':
        // TODO: generate a BASIC FOR loop when it's worth it
        for (;bunch_size; --bunch_size)
        {
            start_line();
            emitB(0x99); // PRINT
            emitB(' ');
            emitB(0xc7); // CHR$
            emitS("(m%(p%));");
            end_line();
        }
        break;
    }
}

static unsigned int loop_lines[20], loop_pcs[20];
static unsigned int *loop_line = loop_lines, *loop_pc = loop_pcs;

static char bunch_size = 0;
static char last_instr = 0;

void transpile_init (unsigned int prog_origin)
{
    pc = prog_origin;
    basic_line = 0;

    emitB(0);
    start_line();
    emitB(0x8f); // REM
    emitS(" brainfuck");
    end_line();

    start_line();
    emitB(0x86); // DIM;
    emitS(" m%(255)");
    rem("memory");
    end_line();

    start_line();
    emitS("p%");
    emitB(0xb2); // =
    emitS("0");
    rem("pointer");
    end_line();


}

unsigned int transpile_finish ()
{
    transpile_bunch(last_instr, bunch_size);

    start_line();
    emitB(0x80); // END
    end_line ();

    emitW(0);

    return pc;
}

void transpile (char instr)
{
    switch (instr)
    {
    case '+':
    case '-':
    case '>':
    case '<':
    case '.':
        if (last_instr == instr && bunch_size < 255)
        {
            ++bunch_size;
        }
        else
        {
            transpile_bunch(last_instr, bunch_size);
            last_instr = instr;
            bunch_size = 1;
        }
        break;
    case '[':
        transpile_bunch(last_instr, bunch_size);
        last_instr = 0;
        bunch_size = 0;

        *(loop_line++) = basic_line;
        start_line();
        emitB(0x8b); // IF
        emitS(" m%(p%) ");
        emitB(0xb2); // =
        emitS(" 0 ");
        emitB(0xa7); // THEN
        *(loop_pc++) = pc;
        emitS("      "); // will be filled in by ]
        end_line();
        break;

    case ']':
        transpile_bunch(last_instr, bunch_size);
        last_instr = 0;
        bunch_size = 0;

        start_line();
        emitB(0x89); // GOTO
        pc += sprintf((char*)pc, " %u", *(--loop_line));
        sprintf((char*)(*(--loop_pc)), " %5u", basic_line);
        end_line();
        break;
    }
}
