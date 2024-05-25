#ifndef AVRKIT_TIMER_H
#define AVRKIT_TIMER_H

#include <avr/io.h>


/*
 * Interruption handling
 */

#define TIMER_ENABLE_COMPARE_INTERRUPT(TIMER_ID, REGISTER_ID) \
inline static void \
timer ## TIMER_ID ## __enable_compare_ ## REGISTER_ID ## _interrupt() { \
	TIMSK ## TIMER_ID |= 1 << OCIE ## TIMER_ID ## REGISTER_ID; \
}

#define TIMER_DISABLE_COMPARE_INTERRUPT(TIMER_ID, REGISTER_ID) \
inline static void \
timer ## TIMER_ID ## __disable_compare_ ## REGISTER_ID ## _interrupt() { \
	TIMSK ## TIMER_ID &= ~(1 << OCIE ## TIMER_ID ## REGISTER_ID); \
}


/*
 * Mode selection
 */

#define TIMER_DISABLE_COMPARE_MATCH(TIMER_ID, REGISTER_ID) \
inline static void \
timer ## TIMER_ID ## __disable_compare_match_ ## REGISTER_ID () { \
	TCCR ## TIMER_ID ## A &= ~( \
		(1 << COM ## TIMER_ID ## REGISTER_ID ## 0) | \
		(1 << COM ## TIMER_ID ## REGISTER_ID ## 1) \
	); \
}


#define TIMER_TOGGLE_ON_COMPARE_MATCH(TIMER_ID, REGISTER_ID) \
inline static void \
timer ## TIMER_ID ## __toggle_on_compare_match_ ## REGISTER_ID () { \
	TCCR ## TIMER_ID ## A &= ~(1 << COM ## TIMER_ID ## REGISTER_ID ## 1); \
	TCCR ## TIMER_ID ## A |=  (1 << COM ## TIMER_ID ## REGISTER_ID ## 0); \
}


#define TIMER_CLEAR_ON_COMPARE_MATCH(TIMER_ID, REGISTER_ID) \
inline static void \
timer ## TIMER_ID ## __clear_on_compare_match_ ## REGISTER_ID () { \
	TCCR ## TIMER_ID ## A &= ~(1 << COM ## TIMER_ID ## REGISTER_ID ## 0); \
	TCCR ## TIMER_ID ## A |=  (1 << COM ## TIMER_ID ## REGISTER_ID ## 1); \
}


#define TIMER_SET_ON_COMPARE_MATCH(TIMER_ID, REGISTER_ID) \
inline static void \
timer ## TIMER_ID ## __set_on_compare_match_ ## REGISTER_ID () { \
	TCCR ## TIMER_ID ## A |= \
		(1 << COM ## TIMER_ID ## REGISTER_ID ## 0) | \
		(1 << COM ## TIMER_ID ## REGISTER_ID ## 1); \
}


#define TIMER_8BITS_SET_NORMAL_MODE(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __set_normal_mode() { \
	TCCR ## TIMER_ID ## A &= \
		~( \
			(1 << WGM ## TIMER_ID ## 0) | \
			(1 << WGM ## TIMER_ID ## 1) \
		); \
\
	TCCR ## TIMER_ID ## B &= ~(1 << WGM ## TIMER_ID ## 2); \
}

#define TIMER_16BITS_SET_NORMAL_MODE(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __set_normal_mode() { \
	TCCR ## TIMER_ID ## A &= \
		~( \
			(1 << WGM ## TIMER_ID ## 0) | \
			(1 << WGM ## TIMER_ID ## 1) \
		); \
\
	TCCR ## TIMER_ID ## B &= \
		~( \
			(1 << WGM ## TIMER_ID ## 2) | \
			(1 << WGM ## TIMER_ID ## 3) \
		); \
}

#define TIMER_8BITS_SET_CTC_MODE(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __set_CTC_mode() { \
	timer ## TIMER_ID ## __set_normal_mode(); \
	TCCR ## TIMER_ID ## A |= (1 << WGM ## TIMER_ID ## 1); \
}

#define TIMER_16BITS_SET_CTC_MODE(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __set_CTC_mode() { \
	timer ## TIMER_ID ## __set_normal_mode(); \
	TCCR ## TIMER_ID ## B |= (1 << WGM ## TIMER_ID ## 2); \
}

#define TIMER_8BITS_SET_FAST_PWM_MODE(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __set_fast_PWM_mode() { \
	timer ## TIMER_ID ## __set_normal_mode(); \
	TCCR ## TIMER_ID ## A |= \
		(1 << WGM ## TIMER_ID ## 0) | \
		(1 << WGM ## TIMER_ID ## 1); \
}

#define TIMER_8BITS_SET_FAST_PWM_MODE_WITH_OCRA_AS_TOP(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __set_fast_PWM_mode_with_OCRA_as_top() { \
	TCCR ## TIMER_ID ## A |= \
		(1 << WGM ## TIMER_ID ## 0) | \
		(1 << WGM ## TIMER_ID ## 1); \
	TCCR ## TIMER_ID ## B |= \
		(1 << WGM ## TIMER_ID ## 2); \
}



/*
 * Premultiplier handling
 */

#define TIMER_STOP(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __stop() { \
	TCCR ## TIMER_ID ## B &= \
	~( \
		(1 << CS ## TIMER_ID ## 0) | \
		(1 << CS ## TIMER_ID ## 1) | \
		(1 << CS ## TIMER_ID ## 2) \
	); \
}

#define TIMER_START(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __start() { \
	TCCR ## TIMER_ID ## B &= \
	~( \
		(1 << CS ## TIMER_ID ## 1) | \
		(1 << CS ## TIMER_ID ## 2) \
	); \
\
	TCCR ## TIMER_ID ## B |= \
		(1 << CS ## TIMER_ID ## 0); \
}

#define TIMER_START_WITH_PREMULTIPLIER_8(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __start_with_premultiplier_8() { \
	TCCR ## TIMER_ID ## B &= \
	~( \
		(1 << CS ## TIMER_ID ## 0) | \
		(1 << CS ## TIMER_ID ## 2) \
	); \
\
	TCCR ## TIMER_ID ## B |= \
		(1 << CS ## TIMER_ID ## 1); \
}

#define TIMER_START_WITH_EXTENDED_PREMULTIPLIER_32(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __start_with_premultiplier_32() { \
	timer ## TIMER_ID ## __stop(); \
\
	TCCR ## TIMER_ID ## B |= \
		(1 << CS ## TIMER_ID ## 0) | \
		(1 << CS ## TIMER_ID ## 1); \
}

#define TIMER_START_WITH_PREMULTIPLIER_64(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __start_with_premultiplier_64() { \
	timer ## TIMER_ID ## __stop(); \
\
	TCCR ## TIMER_ID ## B |= \
		(1 << CS ## TIMER_ID ## 0) | \
		(1 << CS ## TIMER_ID ## 1); \
}

#define TIMER_START_WITH_EXTENDED_PREMULTIPLIER_64(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __start_with_premultiplier_64() { \
	TCCR ## TIMER_ID ## B &= \
	~( \
		(1 << CS ## TIMER_ID ## 1) | \
		(1 << CS ## TIMER_ID ## 2) \
	); \
\
	TCCR ## TIMER_ID ## B |= \
		(1 << CS ## TIMER_ID ## 2); \
}

#define TIMER_START_WITH_EXTENDED_PREMULTIPLIER_128(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __start_with_premultiplier_128() { \
	timer ## TIMER_ID ## __stop(); \
\
	TCCR ## TIMER_ID ## B |= \
		(1 << CS ## TIMER_ID ## 0) | \
		(1 << CS ## TIMER_ID ## 2); \
}

#define TIMER_START_WITH_PREMULTIPLIER_256(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __start_with_premultiplier_256() { \
	TCCR ## TIMER_ID ## B &= \
	~( \
		(1 << CS ## TIMER_ID ## 1) | \
		(1 << CS ## TIMER_ID ## 2) \
	); \
\
	TCCR ## TIMER_ID ## B |= \
		(1 << CS ## TIMER_ID ## 2); \
}

#define TIMER_START_WITH_PREMULTIPLIER_1024(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __start_with_premultiplier_1024() { \
	timer ## TIMER_ID ## __stop(); \
\
	TCCR ## TIMER_ID ## B |= \
		(1 << CS ## TIMER_ID ## 0) | \
		(1 << CS ## TIMER_ID ## 2); \
}

#define TIMER_START_WITH_EXTENDED_PREMULTIPLIER_256(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __start_with_premultiplier_256() { \
	timer ## TIMER_ID ## __stop(); \
\
	TCCR ## TIMER_ID ## B |= \
		(1 << CS ## TIMER_ID ## 1) | \
		(1 << CS ## TIMER_ID ## 2); \
}

#define TIMER_START_WITH_EXTENDED_PREMULTIPLIER_1024(TIMER_ID) \
inline static void \
timer ## TIMER_ID ## __start_with_premultiplier_1024() { \
	TCCR ## TIMER_ID ## B |= \
		(1 << CS ## TIMER_ID ## 0) | \
		(1 << CS ## TIMER_ID ## 1) | \
		(1 << CS ## TIMER_ID ## 2); \
}


/*
 * Instanciate all the timer definitions
 */

#define INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(TIMER_ID, REGISTER_ID) \
	TIMER_ENABLE_COMPARE_INTERRUPT(TIMER_ID, REGISTER_ID) \
	TIMER_DISABLE_COMPARE_INTERRUPT(TIMER_ID, REGISTER_ID)

#define INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(TIMER_ID, REGISTER_ID) \
	TIMER_DISABLE_COMPARE_MATCH(TIMER_ID, REGISTER_ID) \
	TIMER_TOGGLE_ON_COMPARE_MATCH(TIMER_ID, REGISTER_ID) \
	TIMER_CLEAR_ON_COMPARE_MATCH(TIMER_ID, REGISTER_ID) \
	TIMER_SET_ON_COMPARE_MATCH(TIMER_ID, REGISTER_ID)
	
#define INSTANCIATE_TIMER_8BITS_MODE_SETTING_DEFINITIONS(TIMER_ID) \
	TIMER_8BITS_SET_NORMAL_MODE(TIMER_ID) \
	TIMER_8BITS_SET_CTC_MODE(TIMER_ID) \
	TIMER_8BITS_SET_FAST_PWM_MODE(TIMER_ID) \
	TIMER_8BITS_SET_FAST_PWM_MODE_WITH_OCRA_AS_TOP(TIMER_ID)


#define INSTANCIATE_TIMER_16BITS_MODE_SETTING_DEFINITIONS(TIMER_ID) \
	TIMER_16BITS_SET_NORMAL_MODE(TIMER_ID) \
	TIMER_16BITS_SET_CTC_MODE(TIMER_ID)


#define INSTANCIATE_TIMER_PREMULTIPLIER_DEFINITIONS(TIMER_ID) \
	TIMER_STOP(TIMER_ID) \
	TIMER_START(TIMER_ID) \
	TIMER_START_WITH_PREMULTIPLIER_8(TIMER_ID) \
	TIMER_START_WITH_PREMULTIPLIER_64(TIMER_ID) \
	TIMER_START_WITH_PREMULTIPLIER_256(TIMER_ID) \
	TIMER_START_WITH_PREMULTIPLIER_1024(TIMER_ID)


#define INSTANCIATE_TIMER_EXTENDED_PREMULTIPLIER_DEFINITIONS(TIMER_ID) \
	TIMER_STOP(TIMER_ID) \
	TIMER_START(TIMER_ID) \
	TIMER_START_WITH_PREMULTIPLIER_8(TIMER_ID) \
	TIMER_START_WITH_EXTENDED_PREMULTIPLIER_32(TIMER_ID) \
	TIMER_START_WITH_EXTENDED_PREMULTIPLIER_64(TIMER_ID) \
	TIMER_START_WITH_EXTENDED_PREMULTIPLIER_128(TIMER_ID) \
	TIMER_START_WITH_EXTENDED_PREMULTIPLIER_256(TIMER_ID) \
	TIMER_START_WITH_EXTENDED_PREMULTIPLIER_1024(TIMER_ID)


#ifdef __AVRKIT_atmega328p__

INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(0, A)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(0, B)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(1, A)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(1, B)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(2, A)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(2, B)

INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(0, A)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(0, B)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(1, A)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(1, B)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(2, A)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(2, B)

INSTANCIATE_TIMER_8BITS_MODE_SETTING_DEFINITIONS(0)
INSTANCIATE_TIMER_16BITS_MODE_SETTING_DEFINITIONS(1)
INSTANCIATE_TIMER_8BITS_MODE_SETTING_DEFINITIONS(2)
 
INSTANCIATE_TIMER_PREMULTIPLIER_DEFINITIONS(0)
INSTANCIATE_TIMER_PREMULTIPLIER_DEFINITIONS(1)
INSTANCIATE_TIMER_EXTENDED_PREMULTIPLIER_DEFINITIONS(2)

#endif

#ifdef __AVRKIT_atmega32u4__

INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(0, A)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(0, B)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(1, A)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(1, B)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(2, A)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(2, B)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(3, A)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(3, B)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(4, A)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(4, B)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(5, A)
INSTANCIATE_TIMER_INTERRUPT_DEFINITIONS(5, B)

INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(0, A)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(0, B)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(1, A)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(1, B)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(2, A)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(2, B)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(3, A)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(3, B)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(4, A)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(4, B)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(5, A)
INSTANCIATE_TIMER_COMPARE_MATCH_DEFINITIONS(5, B)

INSTANCIATE_TIMER_8BITS_MODE_SETTING_DEFINITIONS(0)
INSTANCIATE_TIMER_16BITS_MODE_SETTING_DEFINITIONS(1)
INSTANCIATE_TIMER_8BITS_MODE_SETTING_DEFINITIONS(2)
INSTANCIATE_TIMER_16BITS_MODE_SETTING_DEFINITIONS(3)
INSTANCIATE_TIMER_16BITS_MODE_SETTING_DEFINITIONS(4)
INSTANCIATE_TIMER_16BITS_MODE_SETTING_DEFINITIONS(5)

INSTANCIATE_TIMER_PREMULTIPLIER_DEFINITIONS(0)
INSTANCIATE_TIMER_PREMULTIPLIER_DEFINITIONS(1)
INSTANCIATE_TIMER_EXTENDED_PREMULTIPLIER_DEFINITIONS(2)
INSTANCIATE_TIMER_PREMULTIPLIER_DEFINITIONS(3)
INSTANCIATE_TIMER_PREMULTIPLIER_DEFINITIONS(4)
INSTANCIATE_TIMER_PREMULTIPLIER_DEFINITIONS(5)

#endif


#endif /* AVRKIT_TIMER_H */