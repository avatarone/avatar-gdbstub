
CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
CFLAGS = -Wall -O0 
LDFLAGS = -nostdlib -nostdinc -nostartfiles
OBJCOPY = arm-none-eabi-objcopy


inforad_k0_hello_world.o: CFLAGS += -mcpu=arm7tdmi -include startup_inforad_k0.h -include uart_sirf.h
inforad_k0_hello_world.o: LDFLAGS += -mcpu=arm7tdmi -Ttext=0x0

all: inforad_k0_hello_world.bin

%.bin: %.elf
	$(OBJCOPY) -Obinary $< $@

%.elf: %.o
	$(LD) $(LDFLAGS) -o $@ $^

%_hello_world.o:  hello_world.c
	$(CC) $(CFLAGS) -c -o $@ $^
	
.PHONY: clean
clean:
	@rm -f *.bin *.elf *.o

