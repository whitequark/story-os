include ../template.mk

all:	api_file.o system_fs.o fs.o tty.o tty null.o null
	@echo "  Linking [CUST]  fs"
	@ld -melf_i386 -nostdlib -Ttext 0x10001000 $(LIB) $(ROOT)/api/stubs.o $(ROOT)/api/assert.o system_fs.o fs.o -o $(ROOT)/output/fs --oformat elf32-i386
