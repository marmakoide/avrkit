#ifndef AVRKIT_TWI_H
#define AVRKIT_TWI_H

/*
 * TWI (aka I2C) helper functions
 *
 * Those functions allow to use TWI communication in blocking,  master to slave 
 * mode, none of this is using interrupts.
 *
 * Initialization is achieved by calling twi_pins_setup() and twi_set_speed()
 */

#include <avrkit/types.h>


#define TWI_FREQ_STANDARD 100000UL
#define TWI_FREQ_FAST     400000UL


extern void
twi_pins_setup();


extern void
twi_set_speed(unsigned long freq);


extern bool
twi_start();


extern void
twi_stop();


extern bool
twi_request_transmission(uint8_t address);


extern bool
twi_request_reception(uint8_t address);


extern bool
twi_transmit(uint8_t data);


extern bool
twi_receive_ack(uint8_t* data);


extern uint8_t
twi_receive_nack(uint8_t* data);


#endif /* AVRKIT_TWI_H */
