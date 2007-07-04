include ../template.mk
LIBC=../libc/*.o
API=../api/*.o
SAPI=api_*.o

all:	##api_terminal.o terminal.o terminal api_filesystem.o filesystem.o filesystem

%: %.o
	@echo "Linking '$@'"
	@ld -nostdlib -Ttext 0x10000000 $< $(LIBC) $(API) $(SAPI) -o ../output/$@ --oformat elf32-i386
	