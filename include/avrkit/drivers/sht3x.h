#ifndef AVRKIT_DRIVER_SHT3X_H
#define AVRKIT_DRIVER_SHT3X_H

/*
 * Driver for SHT3x humdity and temperature sensors.
 *
 * The I2C address of the device is set by writing the sht3x_i2c_address 
 * variable
 */

#include <avrkit/types.h>


/*
 * The two possibles I2C address for the SHT3x sensor
 */

#define SHT3X_I2C_DEFAULT_ADDRESS 0x44
#define SHT3X_I2C_ALT_ADDRESS     0x45


/*
 * Global variable to store the I2C address for the SHT3x sensor
 * By default, it is set to SHT3X_I2C_DEFAULT_ADDRESS
 */

extern uint8_t 
sht3x_i2c_address;


/*
 * Measurement from a SHT3x sensor
 * Store the values x10, as to avoid working with floating point values
 */

struct sht3x_measure {
	int16_t temperature;
	uint16_t humidity;
}; // struct sht3x_measure


/*
 * Driver API
 */

enum sht3x_measure_repeatability {
	sht3x_measure_repeatability_low    = 0,
	sht3x_measure_repeatability_medium = 1,
	sht3x_measure_repeatability_high   = 2,
}; // enum sht3x_measure_repeatability


enum sht3x_measure_freq {
	sht3x_measure_freq_05hz = 0,
	sht3x_measure_freq_1hz  = 1,
	sht3x_measure_freq_2hz  = 2,
	sht3x_measure_freq_4hz  = 3,
	sht3x_measure_freq_10hz = 4,
}; // enum sht3x_measure_freq


/*
 * Reset the SHT3x sensor
 */

extern bool
sht3x_soft_reset();


/*
 * Read the status register
 */

extern bool
sht3x_read_status(uint16_t* status);


/*
 * Clear the status register
 */

extern bool
sht3x_clear_status();


/*
 * Enable the on-board heater
 */

extern bool
sht3x_enable_heater();


/*
 * Disable the on-board heater
 */

extern bool
sht3x_disable_heater();


/*
 * Request a single shot measurement.
 * The measurement is retrieved with sht3x_read_single_shot_measure().
 */

extern bool
sht3x_request_single_shot_measure(
	enum sht3x_measure_repeatability repeatability
);


/*
 * Read a measurement requested with sht3x_request_single_shot_measure()
 * For this read to be successful, you'll need to wait long enough for the 
 * measurement to be completed, check thet SHT3x datasheet for the timings
 */

extern bool
sht3x_read_single_shot_measure(struct sht3x_measure* out);


/*
 * Request periodic measurements.
 * The measurement is retrieved with sht3x_read_periodic_measure().
 */

extern bool
sht3x_start_periodic_measure(
	enum sht3x_measure_repeatability repeatability,
	enum sht3x_measure_freq freq
);


/*
 * Read a measurement requested with sht3x_read_periodic_measure()
 * For this read to be successful, you'll need to wait long enough for the 
 * measurement to be completed, check thet SHT3x datasheet for the timings
 */

extern bool
sht3x_read_periodic_measure(struct sht3x_measure* out);


/*
 * Stop periodic measurements.
 */

extern bool
sht3x_stop_periodic_measure();


#endif /* AVRKIT_DRIVER_SHT3X_H */
