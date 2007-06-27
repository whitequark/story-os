include ../template.mk
LIBC=../libc/*.o
API=../api/*.o

all:	api_terminal.o terminal.o terminal

%: %.o
	@echo "Linking '$@'"
	@ld -nostdlib -Ttext 0x40000000 $< $(LIBC) $(API) -o ../output/$@ --oformat elf32-i386
	