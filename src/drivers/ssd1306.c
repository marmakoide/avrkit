#include <avr/sfr_defs.h>
#include <avrkit/TWI.h>
#include <avrkit/drivers/ssd1306.h>


/*
 * SSD1306 definitions, following names from the datasheet
 */

#define SSD1306_COMMAND          0x80
#define SSD1306_COMMAND_STREAM   0x00
#define SSD1306_DATA             0xc0
#define SSD1306_DATA_STREAM      0x40

#define SSD1306_SET_MUX_RATIO    0xa8
#define SSD1306_DISPLAY_OFFSET   0xd3
#define SSD1306_DISPLAY_ON       0xaf
#define SSD1306_DISPLAY_OFF      0xae
#define SSD1306_DIS_ENT_DISP_ON  0xa4
#define SSD1306_DIS_IGNORE_RAM   0xa5
#define SSD1306_DIS_NORMAL       0xa6
#define SSD1306_DIS_INVERSE      0xa7
#define SSD1306_DEACT_SCROLL     0x2e
#define SSD1306_ACTIVE_SCROLL    0x2f
#define SSD1306_SET_START_LINE   0x40
#define SSD1306_MEMORY_ADDR_MODE 0x20
#define SSD1306_SET_COLUMN_ADDR  0x21
#define SSD1306_SET_PAGE_ADDR    0x22
#define SSD1306_SEG_REMAP        0xa0
#define SSD1306_SEG_REMAP_OP     0xa1
#define SSD1306_COM_SCAN_DIR     0xc0
#define SSD1306_COM_SCAN_DIR_OP  0xc8
#define SSD1306_COM_PIN_CONF     0xda
#define SSD1306_SET_CONTRAST     0x81
#define SSD1306_SET_OSC_FREQ     0xd5
#define SSD1306_SET_CHAR_REG     0x8d
#define SSD1306_SET_PRECHARGE    0xd9
#define SSD1306_VCOM_DESELECT    0xdb

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2a
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xa3


/*
 * I2C address for the display
 */
 
uint8_t 
ssd1306_i2c_address = SSD1306_I2C_DEFAULT_ADDRESS;


/*
 * Display configurations
 */

const __flash struct ssd1306_display_configuration
ssd1306_128x32_display_conf = {
	0x1f, // mux_ratio
	0x02, // com_pin_conf
	128,  // width
	32,   // height
	512   // framebuffer_size
};

const __flash struct ssd1306_display_configuration
ssd1306_128x64_display_conf = {
	0x3f, // mux_ratio
	0x12, // com_pin_conf
	128,  // width
	64,   // height
	1024  // framebuffer_size
};


const __flash struct ssd1306_display_configuration*
ssd1306_display_conf = &ssd1306_128x64_display_conf;


static bool
ssd1306_send_command(uint8_t command, const uint8_t* data, uint8_t data_size) {
	// Send START
	if (!twi_start())
		return 0;

	// Request for a transmission
	if (!twi_request_transmission(ssd1306_i2c_address))
		return 0;

	// Transmit the command
	twi_transmit(SSD1306_COMMAND);
	twi_transmit(command);

	if (data)
		for( ; data_size != 0; --data_size)
			if (!twi_transmit(*data++))
				return 0;
	
	// Send stop
	twi_stop();

	// Job done
	return 1;
}


static bool
ssd1306_send_command_stream(const uint8_t* data, uint16_t data_size) {
	// Send START
	if (!twi_start())
		return 0;

	// Request for a transmission
	if (!twi_request_transmission(ssd1306_i2c_address))
		return 0;

	// Send commands
	if (!twi_transmit(SSD1306_COMMAND_STREAM))
		return 0;

	for( ; data_size != 0; --data_size)
		if (!twi_transmit(*data++))
			return 0;

	// Send stop
	twi_stop();

	// Job done
	return 1;
}


bool
ssd1306_init() {
	// Generic init sequence
	uint8_t 
	init_sequence[] = {
		SSD1306_DISPLAY_OFF,            //  0: Set display off
		SSD1306_SET_OSC_FREQ, 0x80,     //  1: Set display clock to 105 hz	
		SSD1306_SET_MUX_RATIO, 0x3f,    //  3: Set mutiplex ratio to 1/64
		SSD1306_DISPLAY_OFFSET, 0,      //  5: Set display offset to 0
		SSD1306_SET_START_LINE,         //  7: Set display start line
		SSD1306_SET_CHAR_REG, 0x14,     //  8: Set charge pump
		SSD1306_MEMORY_ADDR_MODE, 0x00, // 10:
		SSD1306_SEG_REMAP_OP,           // 12: Set segment Re-Map default
		SSD1306_COM_SCAN_DIR_OP,        // 13: Set COM Output Scan Direction
		SSD1306_COM_PIN_CONF, 0x12,     // 14: Set COM hardware configuration
		SSD1306_SET_CONTRAST, 0xcf,     // 16: Set contrast
		SSD1306_SET_PRECHARGE, 0xc2,    // 18: Set Pre-Charge period 
		SSD1306_VCOM_DESELECT, 0x20,    // 20: Set Deselect Vcomh level
		SSD1306_DIS_ENT_DISP_ON,        // 21: Entire display ON	
		SSD1306_DIS_NORMAL,             // 22: Set normal display	
		SSD1306_DEACT_SCROLL,           // 23: Deactivate scrolling
		SSD1306_DISPLAY_ON,             // 24: Display ON
	};

	// Patch the init sequence for the specifics of the display
	init_sequence[4] = ssd1306_display_conf->mux_ratio;
	init_sequence[15] = ssd1306_display_conf->com_pin_conf;

	// Send the init sequence
	return
	   ssd1306_send_command_stream(init_sequence, sizeof(init_sequence));
}


bool
ssd1306_upload_start() {
	// Send START
	if (!twi_start())
		return 0;

	// Request for a transmission
	if (!twi_request_transmission(ssd1306_i2c_address))
		return 0;

	// Send request for data stream
	if (!twi_transmit(SSD1306_DATA_STREAM))
		return 0;

	// Job done
	return 1;
}


bool
ssd1306_upload_end() {
	// Send stop
	twi_stop();

	// Job done
	return 1;
}


bool
ssd1306_upload_empty_row(uint8_t row_count) {
	for( ; row_count != 0; --row_count)
		for(uint8_t i = ssd1306_display_conf->width; i != 0; --i)
			if (!twi_transmit(0x00))
				return 0;
	
	return 1;
}


bool
ssd1306_upload_charmap_8x8(const __flash uint8_t* font,
                           const char* charmap,
                           uint8_t charmap_height) {
	// Send the bitmap data
	uint8_t charmap_width = ssd1306_display_conf->width / 8;
	
	for(uint8_t i = charmap_height; i != 0; --i) {
		for(uint8_t j = charmap_width; j != 0; --j, ++charmap) {
			uint8_t glyph_id = *charmap & ~_BV(7);
			
			uint8_t mask = 0x00;
			mask -= (*charmap & _BV(7)) != 0;
			
			const __flash uint8_t* glyph_data = font;
			glyph_data += glyph_id * 8;
			
			for(uint8_t k = 8; k != 0; --k, ++glyph_data)
				if (!twi_transmit(*glyph_data ^ mask))
					return 0;
		}
	}

	// Job done
	return 1;
}


bool
ssd1306_upload_charmap_16x16(const __flash uint8_t* font,
                             const char* charmap,
                             uint8_t charmap_height) {
	// Send the bitmap data
	uint8_t charmap_width = ssd1306_display_conf->width / 16;

	for(uint8_t i = charmap_height; i != 0; --i, charmap += charmap_width) {
		for(uint8_t m = 0; m < 32; m += 16) {
			const char* charmap_row = charmap;
			for(uint8_t j = charmap_width; j != 0; --j, ++charmap_row) {
				uint8_t glyph_id = *charmap_row & ~_BV(7);

				uint8_t mask = 0x00;
				mask -= (*charmap & _BV(7)) != 0;

				const __flash uint8_t* glyph_data = font;
				glyph_data += glyph_id * 32 + m;
				
				for(uint8_t k = 16; k != 0; --k, ++glyph_data)
					if (!twi_transmit(*glyph_data ^ mask))
						return 0;
			}
		}
	}
	
	// Job done
	return 1;
}


bool
ssd1306_upload_framebuffer(const __flash uint8_t* bitmap) {
	// Send START
	if (!twi_start())
		return 0;

	// Request for a transmission
	if (!twi_request_transmission(ssd1306_i2c_address))
		return 0;

	// Send request for data stream
	if (!twi_transmit(SSD1306_DATA_STREAM))
		return 0;

	// Send the bitmap data
	const __flash uint8_t* pixel = bitmap;
	for(uint16_t i = ssd1306_display_conf->framebuffer_size; i != 0; --i, ++pixel) {
		if (!twi_transmit(*pixel))
			return 0;
	}

	// Send stop
	twi_stop();

	// Job done
	return 1;
}


bool
ssd1306_set_display_on() {
	return ssd1306_send_command(SSD1306_DISPLAY_ON, 0, 0);
}


bool
ssd1306_set_display_off() {
	return ssd1306_send_command(SSD1306_DISPLAY_OFF, 0, 0);
}


bool
ssd1306_set_normal_display_mode() {
	return ssd1306_send_command(SSD1306_DIS_NORMAL, 0, 0);
}


bool
ssd1306_set_inverse_display_mode() {
	return ssd1306_send_command(SSD1306_DIS_INVERSE, 0, 0);
}


bool
ssd1306_activate_scroll() {
	return ssd1306_send_command(SSD1306_ACTIVE_SCROLL, 0, 0);
}


bool
ssd1306_deactivate_scroll() {
	return ssd1306_send_command(SSD1306_DEACT_SCROLL, 0, 0);
}


bool
ssd1306_setup_horizontal_scroll(
	uint8_t start,
	uint8_t stop,
	int left_to_right,
	enum ssd1306_scroll_speed speed
) {
	uint8_t data[7] = {
		0x00,
		0x00,
		start,
		speed,
		stop,
		0x00,
		0xff
	};
	data[0] = left_to_right ? SSD1306_RIGHT_HORIZONTAL_SCROLL : SSD1306_LEFT_HORIZONTAL_SCROLL;
	
	return ssd1306_send_command_stream(data, sizeof(data));
}


bool
ssd1306_set_vertical_offset(int8_t offset) {
	uint8_t data[2] = {
		SSD1306_DISPLAY_OFFSET,
		offset
	};
	return ssd1306_send_command_stream(data, sizeof(data));
}
