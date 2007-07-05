include ../template.mk
LIB=../lib/*.o
API=../api/*.o
SAPI=
##../services/api_*.o

all:	app.o app idle.o idle

%: %.o
	@echo "Linking '$@'"
	@ld -nostdlib -Ttext 0x10000000 $< $(LIB) $(API) $(SAPI) -o ../output/$@ --oformat elf32-i386
	