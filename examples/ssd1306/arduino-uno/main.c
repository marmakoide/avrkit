#include <avrkit/TWI.h>
#include <avrkit/drivers/ssd1306.h>


extern const __flash uint8_t framebuffer_data[1024];


int
main() {
	// Setup
	twi_pins_setup();
	twi_set_speed(TWI_FREQ_FAST);
	sei();

	// Initialize the display
	ssd1306_i2c_address = SSD1306_I2C_DEFAULT_ADDRESS;
	ssd1306_display_conf = &ssd1306_128x64_display_conf;
	ssd1306_init();

	// Upload a picture
	ssd1306_upload_start();
	ssd1306_upload_framebuffer(framebuffer_data);
	ssd1306_upload_end();
	
	// Sleep forever
	while(1)
	    sleep_mode();
}
