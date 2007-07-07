include ../template.mk
LIB=../lib/*.o
API=../api/*.o
SAPI=../servers/api_*.o

all:	app.o app

%: %.o
	@echo "Linking '$@'"
	@ld -nostdlib -Ttext 0x10000000 $< $(LIB) $(API) $(SAPI) -o ../output/$@ --oformat elf32-i386
	