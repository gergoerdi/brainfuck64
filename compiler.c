#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
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

#define src_buffer_len 1024
char src_buffer[src_buffer_len];
int len = 0;

void compile(unsigned int mem_base, unsigned int pc)
{
    unsigned int loops[20];
    unsigned int *loop = loops;
    unsigned int start_pc;
    char start_lo, start_hi;
    char end_lo, end_hi;

#define emit(b) *((char*)pc++) = b;
#define emit2(b1, b2) emit(b1); emit(b2);
#define emit3(b1, b2, b3) emit(b1); emit(b2); emit(b3);

#define unpack(x, lo, hi) { lo = *((char*)&x); hi = *((char*)&x+1); }

    int i;
    char mem_lo, mem_hi;
    unpack(mem_base, mem_lo, mem_hi);

    // Zero out memory area
    /* emit2(0xa9, 0x17);           // LDA #$17 */
    /* emit3(0x8d, 0x18, 0xd0);     // STA $D018 */
    emit2(0xa9, 0x00);           // LDA #$00
    emit2(0xa2, 0x00);           // LDX #$ff
    emit3(0x9d, mem_lo, mem_hi); // loop: STA MEM,X
    emit(0xca);                  // DEX
    emit2(0xd0, 0xfa);           // BNE loop

    for (i = 0; i < len; ++i)
    {
        switch (src_buffer[i])
        {
        case '+':
            emit3(0xfe, mem_lo, mem_hi); // INC MEM,X
            break;
        case '-':
            emit3(0xde, mem_lo, mem_hi); // DEC MEM,X
            break;

        case '>':
            emit(0xe8); // INX
            break;
        case '<':
            emit(0xca); // DEX
            break;

        case '.':
            emit3(0xbd, mem_lo, mem_hi); // LDA MEM,X
            emit3(0x20, 0xd2, 0xff);     // JSR $FFD2
            break;

        case '[':
            *(loop++) = pc;
            emit3 (0xbd, mem_lo, mem_hi); // LDA MEM,X
            emit2 (0xd0, 0x03);           // BNE +3
            emit3 (0x4c, 0x00, 0x00);     // JMP end
            // end address will be filled in by handler for ']'
            break;

        case ']':
            start_pc = *(--loop);
            unpack(start_pc, start_lo, start_hi);
            emit3 (0x4c, start_lo, start_hi); // JMP start

            unpack(pc, end_lo, end_hi);
            *((char*)start_pc + 6) = end_lo;
            *((char*)start_pc + 7) = end_hi;
            break;
        }
    }

    // RTS
    emit(0x60);
}

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

    compile(0x3000, 0x2000);

    // Clear STOP status
    *((char*)0x0091) = 0;

    return EXIT_SUCCESS;
}
