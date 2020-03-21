/*
 * buz.c
 *
 * Created: 20.02.2019 16:31:33
 *  Author: Demetriusz
 */ 

#include <config.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "buz.h"
#include "tim.h"

timer_t buzzer_timer = 0;
timer_t buzzer_state = 0;

void buzzer_process(void)
{
	if (buzzer_timer < mktime.ms)
	{
		if (system_events&(1<<EV_ON_BUZZER) && buzzer_state==0)
		{
			ON_BUZZER;
			CLEAR_PIN(system_events, EV_ON_BUZZER);
			buzzer_state = 1;
			buzzer_timer += 100;
		}
		else
		{
			OFF_BUZZER;
			buzzer_state = 0;
			buzzer_timer += 50;
		}
	}
}

void init_buzzer(void)
{
	SET_PIN(BUZZER_DDR1, BUZZER_PIN1);
	//SET_PIN(BUZZER_DDR2, BUZZER_PIN2);
	OFF_BUZZER;
}