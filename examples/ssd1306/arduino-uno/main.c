#include <avrkit/TWI.h>
#include <avrkit/timer.h>
#include <avrkit/drivers/ssd1306.h>


extern const __flash uint8_t framebuffer_data[1024];

extern const __flash uint8_t font_data[1024];


// --- Timer setup ------------------------------------------------------------

volatile int16_t tick_counter = 0;

ISR(TIMER1_COMPA_vect) {
	tick_counter += 1;
}

static void
setup_timer1() {
	// Reset timer after 1250 ticks ie. 1/50 sec on a (16/256) clock
	OCR1A = 1249;

	timer1__set_CTC_mode();
	timer1__enable_compare_A_interrupt();
	timer1__start_with_premultiplier_256();
}


static void
sleep_until_n_ticks(int16_t tick_count) {
	tick_counter = 0;
	while(tick_counter < tick_count)
		sleep_mode();
}


// --- Main entry loop --------------------------------------------------------

int
main() {
	// Setup
	setup_timer1();
	twi_pins_setup();
	twi_set_speed(TWI_FREQ_FAST);
	sei();

	// Initialize the display
	ssd1306_i2c_address = SSD1306_I2C_DEFAULT_ADDRESS;
	ssd1306_display_conf = &ssd1306_128x64_display_conf;
	ssd1306_init();

	// Write some text
	ssd1306_upload_start();
	for(uint8_t i = 0; i < 8; ++i)
		ssd1306_upload_charmap_8x8(font_data, "012456789ABCDEF", 1);	
	ssd1306_upload_end();	

	// Do nothing for a while
	sleep_until_n_ticks(200);
	
	// Upload a picture
	ssd1306_upload_start();
	ssd1306_upload_framebuffer(framebuffer_data);
	ssd1306_upload_end();

	// Do nothing for a while
	sleep_until_n_ticks(200);

	// Flash the screen content for a while
	tick_counter = 0;
	for(uint8_t count = 30; count != 0; --count) {
		while(tick_counter < 15)
			sleep_mode();
		tick_counter -= 15;

		if (count % 2 == 0)
			ssd1306_set_inverse_display_mode();
		else
			ssd1306_set_normal_display_mode();

	}

	// Do nothing for a while
	sleep_until_n_ticks(200);
	
	// Scroll the screen content for a while
	ssd1306_setup_horizontal_scroll(0, 7, 0, ssd1306_scroll_speed__5);
	ssd1306_activate_scroll();
	sleep_until_n_ticks(3000);	
	ssd1306_deactivate_scroll();
	
	// Sleep forever
	while(1)
	    sleep_mode();
}
