/*
 * Blinks the on-board LED of an Arduino UNO board, using a timer instead of
 * a busy loop
 */

#include <util/delay.h>

#include <avrkit/GPIO.h>
#include <avrkit/timer.h>


 ISR(TIMER1_COMPA_vect) {
 	gpio_pin_B5__toggle();
 }


static void
setup_timer1() {
	// Clear timer on compare match
	TCCR1B |= _BV(WGM12);

	// Reset timer after 15625 ticks ie. 1 sec on a 16 / 1024 Mhz clock
	OCR1A = 15624;

	timer1__enable_compare_A_interrupt();
	timer1__start_with_premultiplier_1024();
} 


int
main(void) {
	gpio_pin_B5__set_as_output();
	setup_timer1();
	sei();

	while(1)
		sleep_mode();
}
