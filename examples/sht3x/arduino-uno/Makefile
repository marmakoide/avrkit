include ../../../config.mk
include ../../../usb-settings.mk

INCLUDES=../../../include
LIBAVRKIT=../../../libavrkit.a

.PHONY: clean upload

all: main.hex

%.o: %.c
	$(CC) -I$(INCLUDES) -o $@ -c $(CFLAGS) $<

%.elf: %.o
	$(CC) -mmcu=$(MCU) $< $(LIBAVRKIT) -o $@

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

clean:
	rm -f *.o *.elf *.hex

upload: main.hex
	avrdude -F -V -c arduino -p $(MCU) -P $(USB_PORT) -b $(USB_BAUDS) -U flash:w:$<
