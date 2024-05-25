#include <stdio.h>
#include <avrkit/TWI.h>
#include <avrkit/timer.h>
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
	// Reset timer after 1250 ticks ie. 1/50 sec on a (16/256) clock
	OCR1A = 1249;
	
	// Trigger TIMER1_COMPA interruption
	timer1__set_CTC_mode();
	timer1__enable_compare_A_interrupt();
	timer1__start_with_premultiplier_256();
}


// --- Send measure to USART --------------------------------------------------

void
output_measurement(struct sht3x_measure* measure) {
	fprintf(&usart0_output, "  %d.%uc %u.%u%%\r\n",
		measure->temperature / 10,
		measure->temperature % 10,
		measure->humidity / 10,
		measure->humidity % 10
	);
}


// --- Loop using the single shot measurement mode ----------------------------

bool
single_shot_measurement__start() {
	if (!sht3x_request_single_shot_measure(sht3x_measure_repeatability_high)) {
		fputs("sht3x_request_single_shot_measure failure\r\n", &usart0_output);
		return 0;
	}

	return 1;
}


bool
single_shot_measurement__loop() {
	// Every 50 ticks, ie. every second
	if (tick_counter >= 50) {
		tick_counter -= 50;
		
		// Read latest measure
		struct sht3x_measure measure;
		if (!sht3x_read_single_shot_measure(&measure))
			fputs("sht3x_acquire_measure failure\r\n", &usart0_output);
		else {
			// Initiate the next measure
			if (!sht3x_request_single_shot_measure(sht3x_measure_repeatability_high)) {
				fputs("sht3x_request_single_shot_measure failure\r\n", &usart0_output);
				return 0;
			}

			// Send the measures over the UART
			output_measurement(&measure);
		}
	}

	return 1;
}


// --- Loop using the periodic measurement mode -------------------------------

bool
periodic_measurement__start() {
	if (!sht3x_start_periodic_measure(sht3x_measure_repeatability_high, sht3x_measure_freq_1hz)) {
		fputs("sht3x_start_periodic_measure failure\r\n", &usart0_output);
		return 0;
	}

	return 1;
}


bool
periodic_measurement__loop() {
	// Every 52 ticks ie. 1 sec + 20 msec (SHT3x need 13.5 msec in high repeatability modestop acquisition in periodic mode)
	if (tick_counter >= 49) {
		tick_counter -= 49;
		
		// Read latest measure
		struct sht3x_measure measure;
		if (sht3x_read_periodic_measure(&measure))
			output_measurement(&measure);
		else
			fputs("sht3x_read_periodic_measure failure\r\n", &usart0_output);
	}

	return 1;
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

	// Display status
	uint16_t status = 0;
	if (!sht3x_read_status(&status))
		fputs("sht3x_read_status failure\r\n", &usart0_output);
	else
		fprintf(&usart0_output, "status = 0x%04\r\n", status);
	
	// Main loop
	fputs("starting acquisition loop ...\r\n", &usart0_output);	
	/*
	if (single_shot_measurement__start()) {
		while(single_shot_measurement__loop())
			sleep_mode();
	}
	*/
	if (periodic_measurement__start()) {
		while(periodic_measurement__loop())
			sleep_mode();
	}
	
	// Wait, do nothing loop
	fputs("acquisition loop stopped\r\n", &usart0_output);
	while(1)
	    sleep_mode();
}
