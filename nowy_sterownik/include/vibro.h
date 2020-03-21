/*
 * vibro.h
 *
 * Created: 26.09.2019 17:39:03
 *  Author: Demetriusz
 */ 


#ifndef VIBRO_H_
#define VIBRO_H_
#include "config.h"

#if CONFIG_DEVICE_SOLARKA

#define VIBRO_PORT PORTD
#define VIBRO_DDR DDRD
#define VIBRO_PIN 5

#define VIBRO_INIT_PIN	SET_PIN(VIBRO_DDR, VIBRO_PIN)
#define ON_VIBRO_PIN	SET_PIN(VIBRO_PORT, VIBRO_PIN)
#define OFF_VIBRO_PIN	CLEAR_PIN(VIBRO_PORT, VIBRO_PIN)
#define CONFIG_VIBRO_TIME_ON_S 5 //w sekundach

typedef enum
{
	VIBRO_NO_INIT,
	VIBRO_INIT,
	VIBRO_ON_TIMEOUT,
	VIBRO_ENABLE_TIME,
	VIBRO_ON_PERIOD,
	VIBRO_ON,
	VIBRO_OFF,
	VIBRO_ERROR
}vibro_state_t;

void vibro_init(void);
void vibro_deInit(void);
int vibro_on_time(uint32_t time_ms);
int vibro_on(void);
int vibro_is_on(void); //zwraca tylko jezeli status VIBRO_ON
int vibro_off(void);
void vibro_process(void);
timer_t vibro_time_to_timeout_expected(void);
int vibro_on_time_disp(uint32_t time_ms);
int vibro_off_time_disp(void);
uint8_t vibro_get_state(void);

uint8_t vibro_enable_time_on(uint32_t time);
uint8_t vibro_disable_time_on(void);
timer_t vibro_time_to_period_end(void);
int vibro_on_period(uint32_t time_ms);
void vibro_change_period(uint16_t period);
void vibro_set_period(uint16_t period);
int vibro_on_time_timeout(uint32_t time_ms);



#endif //CONFIG_DEVICE_SOLARKA
#endif /* VIBRO_H_ */