include ../../../config.mk
include ../../../usb-settings.mk

INCLUDES=../../../include
LIBAVRKIT=../../../libavrkit.a
SSD1306_TOOL=../../../tools/ssd1306-tool

.PHONY: clean upload

all: main.hex

tetris.c: tetris.png
	$(SSD1306_TOOL) to-framebuffer $< > $@

font-8x8.c: font-8x8.png
	$(SSD1306_TOOL) to-font --font-format=8x8 $< > $@

%.o: %.c
	$(CC) -I$(INCLUDES) -o $@ -c $(CFLAGS) $<

main.elf: main.o font-8x8.o tetris.o
	$(CC) -mmcu=$(MCU) $^ $(LIBAVRKIT) -o $@

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

clean:
	rm -f *.o *.elf *.hex

upload: main.hex
	avrdude -F -V -c arduino -p $(MCU) -P $(USB_PORT) -b $(USB_BAUDS) -U flash:w:$<
