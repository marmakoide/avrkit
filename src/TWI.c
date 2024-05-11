#include <util/twi.h>
#include "avrkit/TWI.h"


#define INSTANCIATE_TWI_PINS_SETUP_FUNC(SDA_PORT_ID, SDA_PIN_ID, SCL_PORT_ID, SCL_PIN_ID) \
void \
twi_pins_setup() { \
	DDR ## SDA_PORT_ID |= 1 << ( DD ## SDA_PORT_ID ## SDA_PIN_ID ); \
	PORT ## SDA_PORT_ID |= 1 << ( PORT ## SDA_PORT_ID ## SDA_PIN_ID ); \
	DDR ## SCL_PORT_ID |= 1 << ( DD ## SCL_PORT_ID ## SCL_PIN_ID ); \
	PORT ## SCL_PORT_ID |= 1 << ( PORT ## SCL_PORT_ID ## SCL_PIN_ID ); \
}

#ifdef __AVRKIT_atmega328p__
INSTANCIATE_TWI_PINS_SETUP_FUNC(C, 4, C, 5)
#endif

#ifdef __AVRKIT_atmega32u4__
INSTANCIATE_TWI_PINS_SETUP_FUNC(D, 2, D, 3)
#endif


void
twi_set_speed(unsigned long freq) {
	TWBR = (uint8_t)(((F_CPU / freq) - 16) / 2);
}


bool
twi_start() {
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	loop_until_bit_is_set(TWCR, TWINT);
	
	if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
		return 0;
	
	return 1;
}


void
twi_stop() {
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}


bool
twi_request_transmission(uint8_t address) {
	TWDR = (address << 1) | TW_WRITE;
	TWCR = _BV(TWINT) | _BV(TWEN);
	loop_until_bit_is_set(TWCR, TWINT);
	
	if (TW_STATUS != TW_MT_SLA_ACK)
		return 0;

	return 1;
}


bool
twi_request_reception(uint8_t address) {
	TWDR = (address << 1) | TW_READ;
	TWCR = _BV(TWINT) | _BV(TWEN);
	loop_until_bit_is_set(TWCR, TWINT);
	
	if (TW_STATUS != TW_MR_SLA_ACK)
		return 0;

	return 1;
}


bool
twi_transmit(uint8_t data) {
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN);
	loop_until_bit_is_set(TWCR, TWINT);

	if (TW_STATUS != TW_MT_DATA_ACK)
		return 0;

	return 1;
}


bool
twi_receive_ack(uint8_t* data) {
	TWCR = _BV(TWINT) | _BV(TWEN) | (1 << TWEA);
	loop_until_bit_is_set(TWCR, TWINT);

	if (TW_STATUS != TW_MR_DATA_ACK)
		return 0;
	
	*data = TWDR;

	return 1;
}


bool
twi_receive_nack(uint8_t* data) {
	TWCR = _BV(TWINT) | _BV(TWEN);
	loop_until_bit_is_set(TWCR, TWINT);

	if (TW_STATUS != TW_MR_DATA_NACK)
		return 0;
	
	*data = TWDR;

	return 1;
}