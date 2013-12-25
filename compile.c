#include "compile.h"

void compile(char *src_buffer, int src_len, unsigned int mem_base, unsigned int pc)
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

    for (i = 0; i < src_len; ++i)
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

    emit(0x60); // RTS
}
