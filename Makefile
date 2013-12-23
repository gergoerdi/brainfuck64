C_SRCS		= $(wildcard *.c)
ASM_SRCS	= $(wlidcard *.s)
OBJS		= $(patsubst %.c, %.o, $(C_SRCS)) $(patsubst %.s, %.o, $(ASM_SRC))
PRG		= bf64.prg

all: $(PRG)

clean:
	rm -f $(OBJS)  $(PRG)

%.o: %.c
	cl65 -t c64 -c $< -o $@

%.o: %.s
	cl65 -t c64 -c $< -o $@

%.prg: $(OBJS)
	cl65 -t c64 $^ -o $@
