include ../template.mk
LIBC=../libc/*.o
API=../api/*.o

all:	keyboard.o keyboard screen.o screen app.o app

%: %.o
	@echo "Linking '$@'"
	@ld -nostdlib -Ttext 0x80000000 $< $(LIBC) $(API) -o ../output/$@ --oformat elf32-i386
	