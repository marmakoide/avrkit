#include <stdio.h>
#include <avrkit/TWI.h>
#include <avrkit/drivers/sht3x.h>


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


// --- Timer setup ------------------------------------------------------------

volatile uint8_t tick_counter = 0;

ISR(TIMER1_COMPA_vect) {
	tick_counter += 1;
}

static void
setup_timer1() {
	// Clear timer on compare match
	TCCR1B |= _BV(WGM12);

	// Set prescaler to 256
	TCCR1B |= _BV(CS12);

	// Reset timer after 1250 ticks ie. 1/50 sec on a (16/256) clock
	OCR1A = 1249;
	
	// Trigger TIMER1_COMPA interruption
	TIMSK1 |= _BV(OCIE1A);
}


// --- Main entry point -------------------------------------------------------

int
main() {
	// Setup
	setup_timer1();
	usart0__init();
	twi_pins_setup();
	twi_set_speed(TWI_FREQ_FAST);
	sei();

	// Initiate the first measure
	fputs("starting acquisition loop ...\r\n", &usart0_output);
	if (!sht3x_request_single_shot_measure(sht3x_measure_repeatability_high)) {
		fputs("sht3x_request_single_shot_measure failure\r\n", &usart0_output);
		goto waiting_loop;
	}

	// Main loop
	while(1) {
		// Every 200 ticks
		if (tick_counter >= 50) {
			tick_counter -= 50;
			
			// Read latest measure
			struct sht3x_measure measure;
			if (!sht3x_acquire_measure(&measure))
				fputs("sht3x_acquire_measure failure\r\n", &usart0_output);
			else {
				// Initiate the next measure
				if (!sht3x_request_single_shot_measure(sht3x_measure_repeatability_high)) {
					fputs("sht3x_request_single_shot_measure failure\r\n", &usart0_output);
					goto waiting_loop;
				}

				// Send the measures over the UART
				fprintf(&usart0_output, "  %d.%uc %u.%u%%\r\n",
					measure.temperature / 10,
					measure.temperature % 10,
					measure.humidity / 10,
					measure.humidity % 10
				);
			}
		}

		// Enter sleep mode
		sleep_mode();
	}
	
	// Wait, do nothing loop
	waiting_loop:
	while(1)
	    sleep_mode();
}
