CFLAGS=-m32 -w -c -Wall -g2 -nostartfiles -nostdlib -fno-rtti -fno-exceptions -fno-leading-underscore -fno-stack-protector
ROOT=$(HOME)/story-os-code2
INCDIR=-I$(ROOT)/include -I$(ROOT)/lib/include -I$(ROOT)/hal/include -I$(ROOT)/core/include -I$(ROOT)/user/include -I$(ROOT)/core/drivers/include -I$(ROOT)/api/include -I$(ROOT)/servers/include
LIB=$(ROOT)/lib/*.o
API=$(ROOT)/api/*.o
SAPI=$(ROOT)/servers/api_*.o

%.o: %.c
	@gcc $(INCDIR) $(CFLAGS) -o $@ $<
	@echo "  Compiling [C]   $<"

%.o: %.cpp
	@gcc $(INCDIR) $(CFLAGS) -o $@ $<
	@echo "  Compiling [CPP] $<"
	
%.o: %.S
	@gcc $(INCDIR) $(CFLAGS) -o $@ $<
	@echo "  Compiling [ASM] $<"

%: %.o
	@echo "  Linking         $@"
	@ld -melf_i386 -nostdlib -Ttext 0x10000000 $< $(LIB) $(API) $(SAPI) -o $(ROOT)/output/$@ --oformat elf32-i386
