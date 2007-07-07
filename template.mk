CFLAGS=-w -c -Wall -g2 -nostartfiles -nostdlib -fno-rtti -fno-exceptions -fno-leading-underscore
ROOT=/root/story-os/trunk
INCDIR=-I$(ROOT)/include -I$(ROOT)/lib/include -I$(ROOT)/hal/include -I$(ROOT)/core/include -I$(ROOT)/user/include -I$(ROOT)/core/drivers/include -I$(ROOT)/api/include -I$(ROOT)/servers/include

%.o: %.c
	@gcc $(INCDIR) $(CFLAGS) -o $@ $<
	@echo "Compiling $<"

%.o: %.cpp
	@gcc $(INCDIR) $(CFLAGS) -o $@ $<
	@echo "Compiling $<"
	
%.o: %.S
	@gcc $(INCDIR) $(CFLAGS) -o $@ $<
	@echo "Compiling $<"