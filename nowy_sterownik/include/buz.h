/*
 * buz.h
 *
 * Created: 20.02.2019 16:34:46
 *  Author: Demetriusz
 */ 
#include <config.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef BUZ_H_
#define BUZ_H_

#define BUZZER_DDR1 DDRB
#define BUZZER_PORT1 PORTB
#define BUZZER_PIN1 0

#define BUZZER_DDR2 DDRD
#define BUZZER_PORT2 PORTD
#define BUZZER_PIN2 1

#define ON_BUZZER SET_PIN(BUZZER_PORT1, BUZZER_PIN1)
//; SET_PIN(BUZZER_PORT2, BUZZER_PIN2)
#define OFF_BUZZER CLEAR_PIN(BUZZER_PORT1, BUZZER_PIN1)
//; CLEAR_PIN(BUZZER_PORT2, BUZZER_PIN2)
#define ON_BUZZ_SIGNAL SET_PIN(system_events, EV_ON_BUZZER)

void buzzer_process(void);
void init_buzzer(void);




#endif /* BUZ_H_ */