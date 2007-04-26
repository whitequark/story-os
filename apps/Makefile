include ../template.mk
LIBC=../libc/*.o
API=../api/*.o

all:	keyboard.o keyboard app1.o app1 app2.o app2

%: %.o
	@echo "Linking '$@'"
	@ld -nostdlib -Ttext 0x80000000 $< $(LIBC) $(API) -o ../output/$@ --oformat elf32-i386
	