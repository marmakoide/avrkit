include ../../../config.mk
include ../../../usb-settings.mk

INCLUDES=../../../include

.PHONY: clean upload

all: main.hex

%.o: %.c
	$(CC) -I$(INCLUDES) -o $@ -c $(CFLAGS) $<

%.elf: %.o
	$(CC) -mmcu=$(MCU) $< -o $@

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

clean:
	rm -f *.o *.elf *.hex

upload: main.hex
	stty -F $(USB_PORT) ispeed 1200 ospeed 1200 ; \
	avrdude -F -V -c avr109 -p $(MCU) -P $(USB_PORT) -b $(USB_BAUDS) -U flash:w:$<
