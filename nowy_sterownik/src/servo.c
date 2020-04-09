/*
 * pwm.c
 *
 * Created: 07.02.2019 13:19:09
 *  Author: Demetriusz
 */ 
#include <config.h>
#include <stdio.h>
#if !TEST_APP
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#endif //!TEST_APP

#include "tim.h"
//#include "disp.h"
#include "servo.h"
#if CONFIG_DEVICE_SIEWNIK

static void servo_exit_try(void);

sDriver servoD;
static uint8_t try_count = 0;

static void set_pwm(uint16_t pwm)
{
	#if !TEST_APP
	OCR1A = pwm;
	#endif
	//OCR1B = pwm;
}

void servo_set_pwm_val(uint8_t value)
{
	if (value == 0)
	set_pwm(1900);
	else if(value < 50)
	set_pwm(1800 - (value - 10)*7);
	else if(value <= 99)
	set_pwm(1450 - (value - 50)*5);
}

void servo_error(void)
{
	CLOSE_SERVO;
	LED_SERVO_OFF;
	servoD.state = SERVO_ERROR_PROCESS;
}

static void servo_error_process(void)
{
	static evTime timeout;
	if (evTime_is_stated(&timeout))
	{
		if (evTime_check(&timeout) == 1)
		{
			servoD.state = SERVO_ERROR;
			OFF_SERVO;
		}
	}
	else
	{
		evTime_start(&timeout, 2000);
	}
}


void servo_init(uint8_t prescaler)
{
	(void) prescaler;
	#if !TEST_APP
	ICR1 = 19999;
	DDRD |= (1 << 4) | (1<<5); //?????????? TO DO
	// set TOP to 16bit
	OCR1B = 0x0;
	OCR1A = 0x0;
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12)|(1 << WGM13);
	TCCR1A |=  (1<<COM1A1); //(1 << COM1B1) |
	TCCR1B |= (1<<CS11);
	set_pwm(19999);
	LED_SERVO_OFF;
	#endif
	servoD.state = SERVO_CLOSE;
	servoD.value = 0;
	evTime_init(&servoD.timeout);
	servoD.try_cnt = 0;
	try_count = 0;
	debug_msg("SERVO: init\n");
}

int servo_is_open(void)
{
	return servoD.state == SERVO_OPEN || servoD.state == SERVO_DELAYED_OPEN;
}

int servo_delayed_open(uint8_t value)
{
	if (servoD.state == SERVO_CLOSE)
	{
		servoD.state = SERVO_DELAYED_OPEN;
		servoD.value = value;
		debug_msg("SERVO_DELAYED_OPEN %d\n", value);
		return 1;
	}
	/*
	else if (servoD.state == SERVO_TRY)
	{
		servo_exit_try();
		return 1;
	}*/
	else return 0;
}

int servo_open(uint8_t value) // value - 0-100%
{
	if (servoD.state == SERVO_CLOSE || servoD.state == SERVO_OPEN || servoD.state == SERVO_DELAYED_OPEN)
	{
		servoD.state = SERVO_OPEN;
		servoD.value = value;
		servo_set_pwm_val((uint16_t)value);
		debug_msg("SERVO_OPPENED %d\n", value);
		LED_SERVO_ON;
		return 1;
	}
	else if (servoD.state == SERVO_TRY)
	{
		servo_exit_try();
		return 1;
	}
	else return 0;
}

void servo_enable_try(void)
{
	if (servoD.state == SERVO_OPEN || servoD.state == SERVO_CLOSE)
	{
		servoD.last_state = servoD.state;
		servoD.state = SERVO_TRY;
	}
}

int servo_get_try_cnt(void)
{
	return servoD.try_cnt;
}

int servo_close(void)
{
	if (servo_is_open())
	{
		servo_set_pwm_val((uint16_t)0);
		servoD.state = SERVO_CLOSE;
		servoD.value = 0;
		debug_msg("SERVO_CLOSED %d\n", servoD.value);
		LED_SERVO_OFF;
		return 1;
	}
	else if (servoD.state == SERVO_TRY)
	{
		servo_exit_try();
		return 1;
	}
	return 0;
}


void servo_try_reset_timeout(uint32_t time_ms)
{
	evTime_start(&servoD.timeout, time_ms);
}

static void servo_delayed_open_process(void)
{
	static evTime timeout;
	if (evTime_is_stated(&timeout))
	{
		if (evTime_check(&timeout) == 1)
		{
			servoD.state = SERVO_OPEN;
			LED_SERVO_ON;
		}
	}
	else
	{
		evTime_start(&timeout, 2000);
	}
}

static void servo_try_process(void)
{
	static evTime timeout;
	if (try_count == 0)
	{
		evTime_start(&timeout, 150);
		try_count++;
		servo_set_pwm_val(servoD.value + try_count);
	}
	else if (try_count > 0 && try_count < TRY_OPEN_VAL)
	{
		if (evTime_check(&timeout) == 1)
		{
			evTime_start(&timeout, 100);
			try_count++;
			servo_set_pwm_val(servoD.value + try_count*4);
		}
	}
	else
	{
		try_count = 0;
		servo_set_pwm_val(servoD.value);
		servoD.state = servoD.last_state;
		servoD.try_cnt++;
	}
	debug_msg("SERVO_TRY %d\n", servoD.value + try_count);

}

static void servo_exit_try(void)
{
	if (servoD.last_state == SERVO_OPEN)
	{
		servoD.state = SERVO_OPEN;
		servo_close();
	}
	else if ((servoD.last_state == SERVO_CLOSE))
	{
		servoD.state = SERVO_OPEN;
		servo_open(servoD.value);
	}
	else return;
	try_count = 0;
	servoD.try_cnt++;
}

void servo_process(uint8_t value)
{
	static evTime servo_timer;
	if (evTime_process_period(&servo_timer, 75))
	{
		switch(servoD.state)
		{
			case SERVO_OPEN:
			servoD.value = value;
			servo_set_pwm_val((uint16_t)value);
			break;
			case SERVO_TRY:
			servo_try_process();
			break;
			case SERVO_DELAYED_OPEN:
			servo_delayed_open_process();
			break;
			case SERVO_ERROR_PROCESS:
			servo_error_process();
			break;
			case SERVO_ERROR:
			OFF_SERVO;
			break;
		}
		if (evTime_check(&servoD.timeout) == 1) 
		{
			servoD.try_cnt = 0;
			debug_msg("SERVO: Zero try cnt\n");
		}
	}
}

#endif //CONFIG_DEVICE_SIEWNIK








