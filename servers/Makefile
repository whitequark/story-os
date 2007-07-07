include ../template.mk
LIB=../lib/*.o
API=../api/*.o
SAPI=api_*.o

all:	api_filesystem.o api_file.o fs.o fs

%: %.o
	@echo "Linking '$@'"
	@ld -nostdlib -Ttext 0x10000000 $< $(LIB) $(API) $(SAPI) -o ../output/$@ --oformat elf32-i386
	