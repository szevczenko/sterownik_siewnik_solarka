/*
 * vibro.c
 *
 * Created: 26.09.2019 17:40:11
 *  Author: Demetriusz
 */ 
#include "config.h"
#include "vibro.h"
#if CONFIG_DEVICE_SOLARKA
#include "display_d.h"
#include "disp.h"

static vibro_state_t vibro_state = VIBRO_NO_INIT;
static evTime vibro_timeout;

void vibro_init(void)
{
	//init vibro pin
	//debug_msg("VIBRO: init\n");
	VIBRO_INIT_PIN;
	OFF_VIBRO_PIN;
	evTime_init(&vibro_timeout);
	vibro_state = VIBRO_INIT;
	LED_SERVO_OFF;
}

void vibro_deInit(void)
{
	//debug_msg("VIBRO: de_init\n");
	OFF_VIBRO_PIN;
	evTime_off(&vibro_timeout);
	vibro_state = VIBRO_NO_INIT;
	LED_SERVO_OFF;
}

int vibro_on_time(uint32_t time_ms)
{
	if (vibro_state == VIBRO_NO_INIT) return 0;
	//debug_msg("VIBRO: on time ms: %d\n", time_ms);
	vibro_state = VIBRO_ON_TIMEOUT;
	LED_SERVO_ON;
	return 1;
}

int vibro_on_time_disp(uint32_t time_ms)
{
	if (vibro_state == VIBRO_NO_INIT) return 0;
	//debug_msg("VIBRO: vibro_on_time_disp ms: %d\n", time_ms);
	vibro_state = VIBRO_ON_TIMEOUT;
	evTime_start(&vibro_timeout, time_ms);
	LED_SERVO_ON;
	disp_set_state(DISP_SERVO, SEG_VIBRO_TIMEOUT);
	return 1;
}

int vibro_off_time_disp(void)
{
	if (vibro_state == VIBRO_NO_INIT) return 0;
	//debug_msg("VIBRO: vibro_off_time_disp\n");
	vibro_off();
	disp_set_state(DISP_SERVO, SEG_VALUE);
	return 1;
}

timer_t vibro_time_to_timeout_expected(void)
{
	return evTime_time_to_end(&vibro_timeout);
}



int vibro_on(void)
{
	if (vibro_state == VIBRO_NO_INIT) return 0;
	//debug_msg("VIBRO: ON\n");
	vibro_state = VIBRO_ON;
	LED_SERVO_ON;
	return 1;
}

int vibro_is_on(void)
{
	if (vibro_state == VIBRO_ON)
		return 1;
	else
		return 0;
}

int vibro_off(void)
{
	//debug_msg("VIBRO: OFF\n");
	if (vibro_state == VIBRO_NO_INIT) return 0;
	vibro_state = VIBRO_OFF;
	disp_set_state(DISP_SERVO, SEG_VALUE);
	LED_SERVO_OFF;
	return 1;
}

uint8_t vibro_get_state(void)
{
	return vibro_state;
}

static uint8_t vibro_time_state;
static evTime time_timer;
static evTime periodChangedTimer;
static evTime periodOnVibroTime;
static uint16_t time_to_on;

void vibro_change_period(uint16_t period)
{
	if (vibro_state == VIBRO_NO_INIT) return;
	evTime_start(&periodChangedTimer, 2000);
	time_to_on = period;
	display_set_blink(DISP_SERVO, 20, period/100);
}

void vibro_set_period(uint16_t period)
{
	if (vibro_state == VIBRO_NO_INIT) return;
	time_to_on = period;
}

timer_t vibro_time_to_period_end(void)
{
	return evTime_time_to_end(&time_timer);
}

uint8_t vibro_enable_time_on(uint32_t time)
{
	if (vibro_state == VIBRO_NO_INIT) return 0;
	time_to_on = time;
	//debug_msg("VIBRO: vibro_enable_time_on: %d\n", time_to_on);
	vibro_time_state = 1;
	if (!evTime_is_stated(&periodChangedTimer))
		disp_set_state(DISP_SERVO, SEG_VIBRO_PERIOD);
	evTime_start(&time_timer, time_to_on);
	vibro_state = VIBRO_ENABLE_TIME;
	return 1;
}

uint8_t vibro_disable_time_on(void)
{
	if (vibro_state == VIBRO_NO_INIT) return 0;
	vibro_time_state = 0;
	vibro_off();
	disp_set_state(DISP_SERVO, SEG_VALUE);
	evTime_off(&time_timer);
	evTime_off(&periodChangedTimer);
	evTime_off(&periodOnVibroTime);
	vibro_state = VIBRO_OFF;
	return 1;
}

int vibro_on_period(uint32_t time_ms)
{
	if (vibro_state == VIBRO_NO_INIT) return 0;
	//debug_msg("VIBRO: vibro_on_period: %d\n", time_ms);
	vibro_state = VIBRO_ON_PERIOD;
	evTime_start(&periodOnVibroTime, 3000);
	LED_SERVO_ON;
	return 1;
}

int vibro_on_time_timeout(uint32_t time_ms)
{
	if (vibro_state == VIBRO_NO_INIT) return 0;
	//debug_msg("VIBRO: vibro_on_time_timeout: %d\n", time_ms);
	evTime_start(&periodOnVibroTime, 2000);
	vibro_state = VIBRO_ON_PERIOD;
	if (!evTime_is_stated(&periodChangedTimer))
		on_animation(5000, DISP_SERVO); //display_set_blink(DISP_SERVO, 20, 0);
	LED_SERVO_ON;
	return 1;
}

void vibro_process(void)
{
	static evTime process_timer;
	if (evTime_process_period(&process_timer, 250))
	{
		switch (vibro_state)
		{
			case VIBRO_NO_INIT:
			case VIBRO_INIT:
			case VIBRO_ERROR:
			OFF_VIBRO_PIN;
			//ToDo nothing
			break;
			case VIBRO_ON:
				ON_VIBRO_PIN;
			break;
			case VIBRO_ON_TIMEOUT:
				ON_VIBRO_PIN;
				if (evTime_check(&vibro_timeout))
				{
					//debug_msg("VIBRO: timeout expected\n");
					#if CONFIG_VIBRO_TIME_ON
					if (vibro_time_state)
					{
						OFF_VIBRO_PIN;
						LED_SERVO_OFF;
						vibro_state = VIBRO_ENABLE_TIME;
						//debug_msg("VIBRO: start time to on %d\n", time_to_on);
						evTime_start(&time_timer, time_to_on);
					}
					else
					{
						vibro_state = VIBRO_OFF;
					}
					#else
					
					vibro_state = VIBRO_OFF;
					#endif
				}
			break;
			#if CONFIG_VIBRO_TIME_ON
			case VIBRO_ENABLE_TIME:
			OFF_VIBRO_PIN;
			LED_SERVO_OFF;
			if (evTime_check(&time_timer))
			{
				//debug_msg("VIBRO: end VIBRO_ENABLE_TIME\n");
				vibro_on_time_timeout(2000);
			}

			if (evTime_check(&periodChangedTimer))
			{
				disp_set_state(DISP_SERVO, SEG_VIBRO_PERIOD);
			}
			break;

			case VIBRO_ON_PERIOD:
			ON_VIBRO_PIN;
			LED_SERVO_ON;
			if (evTime_check(&periodOnVibroTime))
			{
				//debug_msg("VIBRO: end VIBRO_ON_PERIOD\n");
				if (time_to_on != 0)
					vibro_enable_time_on(time_to_on);
				else
					vibro_on_time_timeout(2000);
			}
			if (evTime_check(&periodChangedTimer))
			{
				on_animation(5000, DISP_SERVO);// display_set_blink(DISP_SERVO, 20, 0);
			}
			break;

			#endif //#if CONFIG_VIBRO_TIME_ON
			case VIBRO_OFF:
				OFF_VIBRO_PIN;
				LED_SERVO_OFF;
			break;
		}
	}
}



#endif //CONFIG_DEVICE_SOLARKA