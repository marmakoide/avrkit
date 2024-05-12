include config.mk

TARGET=libavrkit.a

SRC=./src
BUILD=./build
INCLUDES=./include


.PHONY: clean

all: $(TARGET)

$(BUILD)/%.o: $(SRC)/%.c
	@mkdir --parents $(BUILD)
	$(CC) -I$(INCLUDES) -o $@ -c $(CFLAGS) $<

$(BUILD)/drivers/%.o: $(SRC)/drivers/%.c
	@mkdir --parents $(BUILD)/drivers
	$(CC) -I$(INCLUDES) -o $@ -c $(CFLAGS) $<

libavrkit.a: \
$(BUILD)/TWI.o \
$(BUILD)/drivers/sht3x.o
	$(AR) -r $@ $^

clean:
	@rm -rf $(BUILD)
	@rm -f $(TARGET)

-include $(patsubst $(SRC)/%.c, $(BUILD)/%.deps, $(wildcard $(SRC)/*.c))
-include $(patsubst $(SRC)/drivers/%.c, $(BUILD)/drivers/%.deps, $(wildcard $(SRC)/drivers/*.c))