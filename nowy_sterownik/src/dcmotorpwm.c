/*
dcmotorpwm lib 0x01

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#include <stdio.h>

#include "config.h"

#if !TEST_APP
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

#include "dcmotorpwm.h"

mDriver motorD;
/*
 * init a motor
 */
void dcmotorpwm_init(void) {
	debug_msg("dcmotor init\n");
	evTime_init(&motorD.timeout);
	motorD.state = MOTOR_OFF;
	LED_MOTOR_OFF;
	TCCR2 = 0;
	DCMOTORPWM_DDR |= (1<<DCMOTORPWM_PIN1);
	#if CONFIG_DEVICE_SOLARKA
	CLEAR_PIN(DCMOTORPWM_PORT, DCMOTORPWM_PIN1);
	#else
	SET_PIN(DCMOTORPWM_PORT, DCMOTORPWM_PIN1);
	#endif
}

void dcmotorpwm_deinit(void)
{
	//debug_msg("dcmotor deinit\n");
	motorD.state = MOTOR_NO_INIT;
	#if !TEST_APP
	TCCR2 = 0;
	#if CONFIG_DEVICE_SOLARKA
	CLEAR_PIN(DCMOTORPWM_PORT, DCMOTORPWM_PIN1);
	#else
	SET_PIN(DCMOTORPWM_PORT, DCMOTORPWM_PIN1);
	#endif
	LED_MOTOR_OFF;
	#endif
}

/*
 * stop the motor
 */
int dcmotorpwm_stop(void) {
	
	//set orc
	if (dcmotor_is_on())
	{
		debug_msg("dcmotor stop\n");
		#if !TEST_APP
		TCCR2 = 0;
		CLEAR_PIN(DCMOTORPWM_PORT, DCMOTORPWM_PIN1);
		LED_MOTOR_OFF;
		#endif
		//OCR2 = 0;
		motorD.last_state = motorD.state;
		motorD.state = MOTOR_OFF;
		return 1;
	}
	else
	{
		 //debug_msg("dcmotor cannot stop\n");
	}
	return 0;
}

int dcmotor_is_on(void)
{
	if (motorD.state == MOTOR_ON || motorD.state == MOTOR_AXELERATE || motorD.state == MOTOR_TRY)
	{
		return 1;
	} 
	else return 0;
}

int dcmotorpwm_start(void)
{
	if (motorD.state == MOTOR_OFF)
	{
		//debug_msg("Motor Start\n");
		#if !TEST_APP
		LED_MOTOR_ON;
		
		#if CONFIG_DEVICE_SOLARKA
		TCCR2 |= (1<<COM21); 
		#else
		TCCR2 |= (1<<COM21) | (1<<COM20); 
		#endif
		
		TCCR2 |= (1<<WGM20);
		TCCR2 |= DCMOTORPWM_PRESCALER; //set prescaler
		#endif
		motorD.last_state = motorD.state;
		motorD.state = MOTOR_AXELERATE;
		evTime_start(&motorD.timeout, 1000);
		return 1;
	}
	else 
	{
		//debug_msg("dcmotor canot start\n");
		return 0;
	}
}

static uint8_t count_pwm(int pwm)
{
	#if CONFIG_DEVICE_SOLARKA
	return DCMOTORPWM_MINVEL + (255 - DCMOTORPWM_MINVEL)*pwm/99;
	#else
	return ((DCMOTORPWM_ICR2-DCMOTORPWM_MINVEL) * (pwm / 100.0)) + DCMOTORPWM_MINVEL;
	#endif
}

int dcmotor_set_pwm(int pwm)
{
	if (pwm >= 0 || pwm < 100)
	{
		//debug_msg("dcmotor_set_pwm %d\n", pwm);
		motorD.pwm_value = pwm;
		return 1;
	}
	else
	{
		return 0;
	}
}

int dcmotor_get_pwm(void)
{
	return motorD.pwm_value;
}

void dcmotor_set_error(void)
{
	debug_msg("dcmotor error\n");
	dcmotorpwm_stop();
	motorD.state = MOTOR_ERROR;
}

int dcmotor_set_try(void)
{
	if (dcmotor_is_on())
	{
		motorD.state = MOTOR_TRY;
		return 1;
	}
	return 0;
}

void dcmotor_process(uint8_t value)
{
	static evTime dcmotor_timer;
	if (evTime_process_period(&dcmotor_timer, 150))
	{
		//debug_msg("Process %d\n", motorD.state);
		switch(motorD.state)
		{
			case MOTOR_ON:
			motorD.pwm_value = value;
			OCR2 = count_pwm(value);
			break;

			case MOTOR_OFF:
			motorD.pwm_value = 0;
			break;

			case MOTOR_TRY:
				if (value <= 50)
				{
					motorD.pwm_value = value + 20;
					OCR2 = count_pwm(motorD.pwm_value);
				}
				else if ((value > 50) && (value <= 70))
				{
					motorD.pwm_value = value + 15;
					OCR2 = count_pwm(motorD.pwm_value);
				}
				else
				{
					motorD.pwm_value = value;
					OCR2 = count_pwm(motorD.pwm_value);
				}
			break;

			case MOTOR_ERROR:
			#if !TEST_APP
			TCCR2 = 0;
			CLEAR_PIN(DCMOTORPWM_PORT, DCMOTORPWM_PIN1);
			#endif
			break;

			case MOTOR_AXELERATE:
			motorD.state = MOTOR_ON; //!!
			return;					 //!
			dcmotor_set_pwm(50);
			OCR2 = count_pwm(motorD.pwm_value);
			//debug_msg("MOTOR_AXELERATE %d\n", motorD.pwm_value);
			if (evTime_check(&motorD.timeout))
			{
				motorD.state = MOTOR_ON;
			}
			break;

		}
		
	}
	#if !TEST_APP
	
	#endif
}


