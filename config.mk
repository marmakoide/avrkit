MCU=atmega328p
#MCU=atmega32u4
F_CPU=16000000UL

AR=avr-ar
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-Os -DF_CPU=$(F_CPU) -D__AVRKIT_$(MCU)__ -mmcu=$(MCU)
