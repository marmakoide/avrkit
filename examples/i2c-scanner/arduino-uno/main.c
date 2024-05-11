#include <stdio.h>
#include <avrkit/TWI.h>


// --- USART setup ------------------------------------------------------------

#define ENABLE_USART0_TX_BUFFER 1
#define USART0_TX_BUFFER_SIZE 8

#define ENABLE_USART0_RX_BUFFER 1
#define USART0_RX_BUFFER_SIZE 8

#include <avrkit/USART.h>


ISR(USART_UDRE_vect) {
	usart0__on_data_register_empty();
}
	
ISR(USART_RX_vect) {
	usart0__on_rx_complete();
}


void
usart0__init() {
	usart0__set_bauds(9600UL);
	usart0__set_mode_asynchronous();
	usart0__set_char_size_8();
	usart0__set_stop_bits_1();
	usart0__set_parity_none();
    
	usart0__enable_rx();
	usart0__enable_rx_complete_interrupt();
    
	usart0__enable_tx();
	usart0__enable_data_register_empty_interrupt();
}


int
usart0_putchar(char c, FILE *stream) {
	usart0__send_char(c);
}


FILE usart0_output =
	FDEV_SETUP_STREAM(usart0_putchar, NULL, _FDEV_SETUP_WRITE);


// --- Main entry point -------------------------------------------------------

int
main() {
	uint8_t id_found_count = 0;
	uint8_t id_found_set[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	// Setup
	usart0__init();
	twi_pins_setup();
	twi_set_speed(TWI_FREQ_FAST);
	sei();
	
	// I2C bus scanning
	fputs("scanning I2C bus ...\r\n", &usart0_output);
	for(uint8_t i = 1; i < 128; ++i) {
	    // Send START message on the I2C bus
	    if (!twi_start()) {
	        fputs("  => I2C 'start' message failed\r\n", &usart0_output);
	        goto waiting_loop;
	    }
	    
	    // Send slave address on the I2C bus
	    if (twi_request_transmission(i)) {
	        id_found_set[i / 8] |= 1 << (i % 8);
	        id_found_count += 1;
	    }
	}
	fputs("\r\nscanning complete\r\n", &usart0_output);
	
	// List the devices found
	if (id_found_count == 0)
	    fputs("no devices found\r\n", &usart0_output);
	else {
	    fprintf(&usart0_output, "%u device(s) found:\r\n", id_found_count);
	    for(uint8_t i = 1; i < 128; ++i) {
	        if (id_found_set[i / 8] & (1 << (i % 8)))
	            fprintf(&usart0_output, "  0x%02x\r\n", i);
		}
	}
	
	// Wait, do nothing loop
	waiting_loop:
	while(1)
	    sleep_mode();
}
