#ifndef AVRKIT_DRIVER_SHT3X_H
#define AVRKIT_DRIVER_SHT3X_H

/*
 * Driver for SHT3x humdity and temperature sensors.
 *
 * The I2C address of the device is set by writing the sht3x_i2c_address 
 * variable
 */

#include <avrkit/types.h>


#define SHT3X_I2C_DEFAULT_ADDRESS 0x44
#define SHT3X_I2C_ALT_ADDRESS     0x45

extern uint8_t 
sht3x_i2c_address;


enum sht3x_measure_repeatability {
	sht3x_measure_repeatability_low    = 0,
	sht3x_measure_repeatability_medium = 1,
	sht3x_measure_repeatability_high   = 2,
}; // enum sht3x_measure_repeatability


extern bool
sht3x_soft_reset();


extern bool
sht3x_enable_heater();


extern bool
sht3x_disable_heater();


extern bool
sht3x_request_single_shot_measure(
	enum sht3x_measure_repeatability repeatability
);


/*
 * Acquire a measure that have been requested before-hand
 * Temperature and humidity as x10 their values
 */

extern bool
sht3x_acquire_measure(
	int16_t* temperature,
	uint16_t* humidity
);


#endif /* AVRKIT_DRIVER_SHT3X_H */
