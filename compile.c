#include "compile.h"

static unsigned int pc;
static unsigned int mem;

#define emit(b)       *((char*)pc++) = b;
#define emit2(b1, b2) { emit(b1); emit(b2); }
#define emitW(b, w)   { emit(b); *(int*)pc = w; pc += 2; }

static void compile_bunch (char instr, char bunch_size)
{
    if (!bunch_size)
        return;

    switch (instr)
    {
    case '+':
        if (bunch_size < 2)
        {
            for (;bunch_size; --bunch_size)
                emitW(0xfe, mem); // INC MEM,X
        } else {
            emitW(0xbd, mem); // LDA MEM,X
            emit(0x18); // CLC
            emit2(0x69, bunch_size); // ADC n
            emitW(0x9d, mem); // STA MEM,X
        }
        break;

    case '-':
        if (bunch_size < 2)
        {
            for (;bunch_size; --bunch_size)
                emitW(0xde, mem); // DEC MEM,X
        } else {
            emitW(0xbd, mem); // LDA MEM,X
            emit(0x38); // SEC
            emit2(0xE9, bunch_size); // SBC n
            emitW(0x9d, mem); // STA MEM,X
        }
        break;

    case '>':
        if (bunch_size < 5)
        {
            for (;bunch_size; --bunch_size)
                emit(0xe8); // INX
        } else {
            emit(0x8a); // TXA
            emit(0x18); // CLC
            emit2(0x69, bunch_size); // ADC n
            emit(0xaa); // TAX
        }
        break;

    case '<':
        if (bunch_size < 5)
        {
            for (;bunch_size; --bunch_size)
                emit(0xca); // DEX
        } else {
            emit(0x8a); // TXA
            emit(0x38); // SEC
            emit2(0xe9, bunch_size); // SBC n
            emit(0xaa); // TAX
        }
        break;

    case '.':
        emitW(0xbd, mem); // LDA MEM,X
        for (;bunch_size; --bunch_size)
            emitW(0x20, 0xffd2);   // JSR $FFD2
        break;
    }
}

static unsigned int loops[20];
static unsigned int *loop = loops;

static char bunch_size = 0;
static char last_instr = 0;

void compile_init (unsigned int mem_origin, unsigned int prog_origin)
{
    pc = prog_origin;
    mem = mem_origin;

    // Zero out memory area
    emit2(0xa9, 0x00);     // LDA #$00
    emit2(0xa2, 0x00);     // LDX #$00
    emitW(0x9d, mem); // loop: STA MEM,X
    emit(0xca);            // DEX
    emit2(0xd0, 0xfa);     // BNE loop    
}

void compile_finish ()
{
    compile_bunch(last_instr, bunch_size);

    emit(0x60); // RTS
}

void compile (char *src_buffer, int src_len)
{
    unsigned int start_pc;
    int i;

    for (i = 0; i < src_len; ++i)
    {
        char instr = src_buffer[i];

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
                compile_bunch(last_instr, bunch_size);
                last_instr = instr;
                bunch_size = 1;
            }
            break;
        case '[':
            compile_bunch(last_instr, bunch_size);
            last_instr = 0;
            bunch_size = 0;

            *(loop++) = pc;
            emitW(0xbd, mem); // LDA MEM,X
            emit2(0xd0, 0x03);     // BNE +3
            emitW(0x4c, 0x0000);   // JMP end
            // end address will be filled in by handler for ']'
            break;

        case ']':
            compile_bunch(last_instr, bunch_size);
            last_instr = 0;
            bunch_size = 0;

            start_pc = *(--loop);
            emitW(0x4c, start_pc); // JMP start
            *((int*)(start_pc + 6)) = pc;
            break;
        }
    }
}
