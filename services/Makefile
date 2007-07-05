include ../template.mk
LIB=../lib/*.o
API=../api/*.o
SAPI=api_*.o

all:	filesystem.o filesystem

%: %.o
	@echo "Linking '$@'"
	@ld -nostdlib -Ttext 0x10000000 $< $(LIB) $(API) $(SAPI) -o ../output/$@ --oformat elf32-i386
	