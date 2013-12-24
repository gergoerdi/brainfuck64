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

#define emit(b) *((char*)pc++) = b;
#define emit2(b1, b2) emit(b1); emit(b2);
#define emitW(b, w) emit(b); *((int*)pc) = w; pc += 2;

    int i;

    // Zero out memory area
    /* emit2(0xa9, 0x17);     // LDA #$17 */
    /* emitW(0x8d, 0xd018);   // STA $D018 */
    emit2(0xa9, 0x00);     // LDA #$00
    emit2(0xa2, 0x00);     // LDX #$00
    emitW(0x9d, mem_base); // loop: STA MEM,X
    emit(0xca);            // DEX
    emit2(0xd0, 0xfa);     // BNE loop

    for (i = 0; i < len; ++i)
    {
        switch (src_buffer[i])
        {
        case '+':
            emitW(0xfe, mem_base); // INC MEM,X
            break;
        case '-':
            emitW(0xde, mem_base); // DEC MEM,X
            break;

        case '>':
            emit(0xe8); // INX
            break;
        case '<':
            emit(0xca); // DEX
            break;

        case '.':
            emitW(0xbd, mem_base); // LDA MEM,X
            emitW(0x20, 0xffd2);   // JSR $FFD2
            break;

        case '[':
            *(loop++) = pc;
            emitW(0xbd, mem_base); // LDA MEM,X
            emit2(0xd0, 0x03);     // BNE +3
            emitW(0x4c, 0x0000);   // JMP end
            // end address will be filled in by handler for ']'
            break;

        case ']':
            start_pc = *(--loop);
            emitW(0x4c, start_pc); // JMP start
            *((int*)(start_pc + 6)) = pc;
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

    compile(0x3000, 0xc000);

    // Clear STOP status
    *((char*)0x0091) = 0;

    return EXIT_SUCCESS;
}
