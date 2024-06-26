#include <avrkit/TWI.h>
#include <avrkit/drivers/sht3x.h>


/*
 * I2C address for the device
 */

uint8_t 
sht3x_i2c_address = SHT3X_I2C_DEFAULT_ADDRESS;


/*
 * SHD3x definitions, following names from the datasheet
 */

#define SHT3X_MEASURE_SINGLE_SHOT_HIGH_REPEATABILITY_CMD_ID                    0x2400
#define SHT3X_MEASURE_SINGLE_SHOT_HIGH_REPEATABILITY_CLOCK_STRETCHING_CMD_ID   0x2c06
#define SHT3X_MEASURE_SINGLE_SHOT_MEDIUM_REPEATABILITY_CMD_ID                  0x240b
#define SHT3X_MEASURE_SINGLE_SHOT_MEDIUM_REPEATABILITY_CLOCK_STRETCHING_CMD_ID 0x2c0d
#define SHT3X_MEASURE_SINGLE_SHOT_LOW_REPEATABILITY_CMD_ID                     0x2416
#define SHT3X_MEASURE_SINGLE_SHOT_LOW_REPEATABILITY_CLOCK_STRETCHING_CMD_ID    0x2c10
#define SHT3X_START_MEASUREMENT_0_5_MPS_HIGH_REPEATABILITY_CMD_ID              0x2032
#define SHT3X_START_MEASUREMENT_0_5_MPS_MEDIUM_REPEATABILITY_CMD_ID            0x2024
#define SHT3X_START_MEASUREMENT_0_5_MPS_LOW_REPEATABILITY_CMD_ID               0x202f
#define SHT3X_START_MEASUREMENT_1_MPS_HIGH_REPEATABILITY_CMD_ID                0x2130
#define SHT3X_START_MEASUREMENT_1_MPS_MEDIUM_REPEATABILITY_CMD_ID              0x2126
#define SHT3X_START_MEASUREMENT_1_MPS_LOW_REPEATABILITY_CMD_ID                 0x212d
#define SHT3X_START_MEASUREMENT_2_MPS_HIGH_REPEATABILITY_CMD_ID                0x2236
#define SHT3X_START_MEASUREMENT_2_MPS_MEDIUM_REPEATABILITY_CMD_ID              0x2220
#define SHT3X_START_MEASUREMENT_2_MPS_LOW_REPEATABILITY_CMD_ID                 0x222b
#define SHT3X_START_MEASUREMENT_4_MPS_HIGH_REPEATABILITY_CMD_ID                0x2334
#define SHT3X_START_MEASUREMENT_4_MPS_MEDIUM_REPEATABILITY_CMD_ID              0x2322
#define SHT3X_START_MEASUREMENT_4_MPS_LOW_REPEATABILITY_CMD_ID                 0x2329
#define SHT3X_START_MEASUREMENT_10_MPS_HIGH_REPEATABILITY_CMD_ID               0x2737
#define SHT3X_START_MEASUREMENT_10_MPS_MEDIUM_REPEATABILITY_CMD_ID             0x2721
#define SHT3X_START_MEASUREMENT_10_MPS_LOW_REPEATABILITY_CMD_ID                0x273a
#define SHT3X_START_ART_MEASUREMENT_CMD_ID                                     0x2b32
#define SHT3X_READ_MEASUREMENT_CMD_ID                                          0xe000
#define SHT3X_STOP_MEASUREMENT_CMD_ID                                          0x3093
#define SHT3X_ENABLE_HEATER_CMD_ID                                             0x306d
#define SHT3X_DISABLE_HEATER_CMD_ID                                            0x3066
#define SHT3X_READ_STATUS_REGISTER_CMD_ID                                      0xf32d
#define SHT3X_CLEAR_STATUS_REGISTER_CMD_ID                                     0x3041
#define SHT3X_SOFT_RESET_CMD_ID                                                0x30a2


static bool
sht3x_send_command(uint16_t command) {
	// Send START
	if (!twi_start())
		return 0;

	// Request for a transmission
	if (!twi_request_transmission(sht3x_i2c_address))
		return 0;

	// Transmit the command
	if (!twi_transmit(command >> 8))
		return 0;
	
	if (!twi_transmit(command & 0x00ff))
		return 0;
	
	// Send stop
	twi_stop();

	// Job done;
	return 1;
}


static bool
sht3x_read_measure(struct sht3x_measure* out) {
	uint16_t raw_temperature = 0;
	uint16_t raw_humidity = 0;

	uint8_t* raw_temperature_ptr = (uint8_t*)&raw_temperature;
	uint8_t* raw_humidity_ptr = (uint8_t*)&raw_humidity;
	
	// Send START
	if (!twi_start())
		return 0;

	// Request for a reception
	if (!twi_request_reception(sht3x_i2c_address))
		return 0;

	// Read data
	uint8_t crc = 0;
	
	if (!twi_receive_ack(raw_temperature_ptr + 1))
		return 0;

	if (!twi_receive_ack(raw_temperature_ptr))
		return 0;

	if (!twi_receive_ack(&crc))
		return 0;
	
	if (!twi_receive_ack(raw_humidity_ptr + 1))
		return 0;

	if (!twi_receive_ack(raw_humidity_ptr))
		return 0;

	if (!twi_receive_ack(&crc))
		return 0;
	
	// Send stop
	twi_stop();

	// Decode temperature
	int32_t temperature = raw_temperature;
	temperature *= 1750;
	temperature /= 65535;
	temperature -= 450;
	out->temperature = (int16_t)temperature;
	
	// Decode humdity
	uint32_t humidity = raw_humidity;
	humidity *= 1000;
	humidity /= 65535;
	out->humidity = (uint16_t)humidity; 

	// Job done;
	return 1;
}


bool
sht3x_soft_reset() {
	return sht3x_send_command(SHT3X_SOFT_RESET_CMD_ID);
}


bool
sht3x_read_status(uint16_t* status) {
	// Request a status read
	if (!sht3x_send_command(SHT3X_READ_STATUS_REGISTER_CMD_ID))
		return 0;

	// Send START
	if (!twi_start())
		return 0;

	// Request for a reception
	if (!twi_request_reception(sht3x_i2c_address))
		return 0;
	
	// Read data
	uint8_t* status_ptr = (uint8_t*)status;
	
	if (!twi_receive_ack(status_ptr + 1))
		return 0;

	if (!twi_receive_ack(status_ptr))
		return 0;

	uint8_t crc = 0;
	if (!twi_receive_ack(&crc))
		return 0;
	
	// Send stop
	twi_stop();

	// Job done
	return 1;
}


bool
sht3x_clear_status() {
	return sht3x_send_command(SHT3X_CLEAR_STATUS_REGISTER_CMD_ID);
}


bool
sht3x_enable_heater() {
	return sht3x_send_command(SHT3X_ENABLE_HEATER_CMD_ID);
}


extern bool
sht3x_disable_header() {
	return sht3x_send_command(SHT3X_DISABLE_HEATER_CMD_ID);
}



bool
sht3x_request_single_shot_measure(enum sht3x_measure_repeatability repeatability) {
	uint16_t command;
	switch(repeatability) {
		case sht3x_measure_repeatability_low:
			command = SHT3X_MEASURE_SINGLE_SHOT_LOW_REPEATABILITY_CMD_ID;
			break;
			
		case sht3x_measure_repeatability_medium:
			command = SHT3X_MEASURE_SINGLE_SHOT_MEDIUM_REPEATABILITY_CMD_ID;
			break;
			
		case sht3x_measure_repeatability_high:
			command = SHT3X_MEASURE_SINGLE_SHOT_HIGH_REPEATABILITY_CMD_ID;
			break;
	}
	
	return sht3x_send_command(command);
}


bool
sht3x_read_single_shot_measure(struct sht3x_measure* out) {
	return sht3x_read_measure(out);
}


bool
sht3x_start_periodic_measure(
	enum sht3x_measure_repeatability repeatability,
	enum sht3x_measure_freq freq
) {
	uint16_t command;
	switch(freq) {
		case sht3x_measure_freq_05hz:
			switch(repeatability) {
				case sht3x_measure_repeatability_low:
					command = SHT3X_START_MEASUREMENT_0_5_MPS_MEDIUM_REPEATABILITY_CMD_ID;
					break;
					
				case sht3x_measure_repeatability_medium:
					command = SHT3X_START_MEASUREMENT_0_5_MPS_MEDIUM_REPEATABILITY_CMD_ID;
					break;
					
				case sht3x_measure_repeatability_high:
					command = SHT3X_START_MEASUREMENT_0_5_MPS_HIGH_REPEATABILITY_CMD_ID;
					break;
			}
		break;

		case sht3x_measure_freq_1hz:
			switch(repeatability) {
				case sht3x_measure_repeatability_low:
					command = SHT3X_START_MEASUREMENT_1_MPS_MEDIUM_REPEATABILITY_CMD_ID;
					break;
					
				case sht3x_measure_repeatability_medium:
					command = SHT3X_START_MEASUREMENT_1_MPS_MEDIUM_REPEATABILITY_CMD_ID;
					break;
					
				case sht3x_measure_repeatability_high:
					command = SHT3X_START_MEASUREMENT_1_MPS_HIGH_REPEATABILITY_CMD_ID;
					break;
			}
		break;

		case sht3x_measure_freq_2hz:
			switch(repeatability) {
				case sht3x_measure_repeatability_low:
					command = SHT3X_START_MEASUREMENT_2_MPS_MEDIUM_REPEATABILITY_CMD_ID;
					break;
					
				case sht3x_measure_repeatability_medium:
					command = SHT3X_START_MEASUREMENT_2_MPS_MEDIUM_REPEATABILITY_CMD_ID;
					break;
					
				case sht3x_measure_repeatability_high:
					command = SHT3X_START_MEASUREMENT_2_MPS_HIGH_REPEATABILITY_CMD_ID;
					break;
			}
		break;

		case sht3x_measure_freq_4hz:
			switch(repeatability) {
				case sht3x_measure_repeatability_low:
					command = SHT3X_START_MEASUREMENT_4_MPS_MEDIUM_REPEATABILITY_CMD_ID;
					break;
					
				case sht3x_measure_repeatability_medium:
					command = SHT3X_START_MEASUREMENT_4_MPS_MEDIUM_REPEATABILITY_CMD_ID;
					break;
					
				case sht3x_measure_repeatability_high:
					command = SHT3X_START_MEASUREMENT_4_MPS_HIGH_REPEATABILITY_CMD_ID;
					break;
			}
		break;

		case sht3x_measure_freq_10hz:
			switch(repeatability) {
				case sht3x_measure_repeatability_low:
					command = SHT3X_START_MEASUREMENT_10_MPS_MEDIUM_REPEATABILITY_CMD_ID;
					break;
					
				case sht3x_measure_repeatability_medium:
					command = SHT3X_START_MEASUREMENT_10_MPS_MEDIUM_REPEATABILITY_CMD_ID;
					break;
					
				case sht3x_measure_repeatability_high:
					command = SHT3X_START_MEASUREMENT_10_MPS_HIGH_REPEATABILITY_CMD_ID;
					break;
			}
		break;
	}

	return sht3x_send_command(command);
}


bool
sht3x_read_periodic_measure(struct sht3x_measure* out) {
	if (!sht3x_send_command(SHT3X_READ_MEASUREMENT_CMD_ID))
		return 0;

	return sht3x_read_measure(out);
}


bool
sht3x_stop_periodic_measure() {
	return sht3x_send_command(SHT3X_STOP_MEASUREMENT_CMD_ID);
}

