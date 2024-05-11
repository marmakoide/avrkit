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

libavrkit.a: \
$(BUILD)/TWI.o 
	$(AR) -r $@ $^

clean:
	@rm -rf $(BUILD)
	@rm -f $(TARGET)

-include $(patsubst $(SRC)/%.c, $(BUILD)/%.deps, $(wildcard $(SRC)/*.c))