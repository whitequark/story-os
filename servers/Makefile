include ../template.mk

all:	api_file.o system_fs.o fs.o tty.o tty null.o null
	@echo "  Linking [CUST]  fs"
	@ld -nostdlib -Ttext 0x10000000 $(LIB) $(ROOT)/api/stubs.o $(ROOT)/api/assert.o system_fs.o fs.o -o $(ROOT)/output/fs --oformat elf32-i386