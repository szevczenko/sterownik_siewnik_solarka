#include <stdint.h>
#include "measure.h"
#include "accumulator.h"
#include "error.h"
#include "config.h"
///////////////////////////////////////
///TODO get_calibration_value
int get_calibration_value(uint8_t type);
int get_disp_value(uint8_t type);

uint16_t count_error(uint16_t x)
{
    uint16_t error_max_voltage;
    float voltage = accum_get_voltage();
	if (voltage < 9) voltage = 9;
	if (x >=10 && x < 15)
		error_max_voltage = 2*x-15 + (voltage-9)*x/6;
	else if (x >=15 && x < 25)
		error_max_voltage = 2*x-20 + (voltage-9)*x/6;
	else if (x >=25 && x < 40)
		error_max_voltage = 2*x-25 + (voltage-9)*x/6;
	else if (x >=40 && x < 50)
		error_max_voltage = 2*x-10 + (voltage-9)*x/6;
	else if (x >=50 && x < 70)
		error_max_voltage = 2*x+10 + (voltage-9)*x/6;
	else if (x >= 70)
		error_max_voltage = 2*x+20 + (voltage-9)*x/6;
	else
		error_max_voltage = 10;
		
	error_max_voltage = error_max_voltage + get_calibration_value(0);
	return error_max_voltage;
}
#if 0
void error_event(void)
{
	static uint32_t error_event_timer;
	if (error_event_timer < mktime.ms && system_events&(1<<EV_SYSTEM_STATE))
	{

		/////////////////////
		///MOTOR
		//
		ERROR_COUNT_MAX_VOLTAGE;
#if 1//CONFIG_USE_ERROR_MOTOR
		ERROR_COUNT_ERROR;
		if (adc0_motor_filter_value > ERROR_CRITICAL_VOLTAGE) adc0_motor_counter = ERROR_COUNTER_MOTOR_OVF_MAX + 1;
		if (adc0_motor_filter_value > ERROR_MAX_VOLTAGE && adc0_motor_counter <= 20)
		{
			//if (motor_isset_timer != 0 && motor_isset_timer < mktime.ms)
			{
				adc0_motor_counter++;
			}
			motor_try_state = 0;
		}
		else if (adc0_motor_filter_value > ERROR_MAX_VOLTAGE && adc0_motor_counter > 20 && adc0_motor_counter <= ERROR_COUNTER_MOTOR_OVF_MAX)
		{
			////// zmiany
			#if CONFIG_USE_AXELERATE_MOTOR
			if (dcmotor_pwm_value <= 150 && motor_try_state == 0)
			{
				dcmotor_pwm_value += 75;
				OCR2 = dcmotor_pwm_value;
			}
			else if (dcmotor_pwm_value > 150 && motor_try_state == 0)
			{
				dcmotor_pwm_value = 250;
				OCR2 = dcmotor_pwm_value;
			}
			#endif
			/*else
			{
				dcmotor_pwm_value = 125;
				OCR2 = dcmotor_pwm_value;
			}*/
			motor_try_state = 1;
			ERROR_COUNT_MAX_VOLTAGE;
			/*
			for (uint8_t k = 0; k<FILTER_TABLE_SIZE; k++)
				adc0_motor_f_table[k] = ERROR_MAX_VOLTAGE + 30;
			*/
			SET_PIN(system_events, EV_SYSTEM_OVF_MOTOR_CNT);
			//if (motor_isset_timer != 0 && motor_isset_timer < mktime.ms)
			{
				adc0_motor_counter++;
			}
		}
		else if (adc0_motor_filter_value > ERROR_MAX_VOLTAGE && adc0_motor_counter > ERROR_COUNTER_MOTOR_OVF_MAX)
		{
			motor_try_state = 0;
			OCR2 = 0;
			dcmotorpwm_stop();
			LED_MOTOR_OFF;
			SET_PIN(system_events, EV_SYSTEM_ERROR_MOTOR);
			segment1.state = SEG_ERROR;
			segment2.state = SEG_ERROR;
			DispD2.value = 'e';
			DispD1.value = 'r';
			CLOSE_SERVO;
			servo_pwm_value = 0;
			pwm_state = 0;
			LED_SERVO_OFF;
			timeout_servo_off = mktime.ms + 1000;
			disp_set_number(&segment2, ERROR_MOTOR_HIGH_CURRENT);
			off_button();
		}
		else if (!(system_events&(1<<EV_SYSTEM_ERROR_MOTOR)) && adc0_motor_filter_value < ERROR_MAX_VOLTAGE)
		{
			adc0_motor_counter = 0;
			motor_try_state = 0;
			CLEAR_PIN(system_events, EV_SYSTEM_OVF_MOTOR_CNT);
		}
#endif //CONFIG_USE_ERROR_MOTOR
		//////////////////////////
		/// servo
#if CONFIG_USE_ERROR_SERVO
		// testy !!!!!!!!!!!!!!!!!!!!!!!!
		//motor_disp_value = adc0_servo_filter_value;
		SERVO_COUNT_VOL_ERR;
		if (servo_isset_timer < mktime.ms)
		{
			if (adc0_servo_filter_value > SERVO_MAX_VOL_ERROR && servo_try_counter > 1)
			{
				servo_try_counter = TRY_ERROR;
			}
			if (adc0_servo_filter_value > SERVO_VOL_ERROR && adc0_servo_counter <= ERROR_COUNTER_OVF_SERVO && !(system_events&(1<<EV_SYSTEM_SERVO_TRY)) && !(system_events&(1<<EV_SYSTEM_ERROR_SERVO)))
			{
				adc0_servo_counter++;
				timeout_servo = mktime.ms + 2000;
			}
			else if (adc0_servo_filter_value > SERVO_VOL_ERROR && adc0_servo_counter > ERROR_COUNTER_OVF_SERVO && !(system_events&(1<<EV_SYSTEM_SERVO_TRY)))
			{
				SET_PIN(system_events, EV_SYSTEM_SERVO_TRY);
				adc0_servo_counter = 0;
			}
			else if (system_events&(1<<EV_SYSTEM_SERVO_TRY) && servo_try_state == 0)
			{
				servo_try_iteration++;
				servo_set_pwm_val(servo_pwm_value + (servo_try_iteration%9)*6);
				if (servo_try_iteration%9 == 0)
				{
					servo_try_counter++;
					servo_try_state = 1;
					adc0_servo_counter = 0;
					timeout_servo = mktime.ms + 500;
				}
				else timeout_servo = mktime.ms + 2000;
				
			}
			else
			{
				adc0_servo_counter = 0;
			}
		}
		
		if (timeout_servo != 0 && mktime.ms>timeout_servo)
		{
			if (adc0_servo_filter_value < SERVO_VOL_ERROR)
				servo_try_counter = 0;
			timeout_servo = 0;
			adc0_servo_counter = 0;
			servo_try_state = 0;
			CLEAR_PIN(system_events, EV_SYSTEM_SERVO_TRY);
		}
		if (timeout_servo_off != 0 && mktime.ms>timeout_servo_off)
		{
			OFF_SERVO;
			timeout_servo_off = 0;
		}
		if (servo_try_counter >= TRY_ERROR)
		{
			OCR2 = 0;
			dcmotorpwm_stop();
			LED_MOTOR_OFF;
			SET_PIN(system_events, EV_SYSTEM_ERROR_SERVO);
			segment1.state = SEG_ERROR;
			segment2.state = SEG_ERROR;
			DispD2.value = 'e';
			DispD1.value = 'r';
			disp_set_number(&segment2, ERROR_SERVO_HIGH_CURRENT);
			OFF_SERVO;
			LED_SERVO_OFF;
			off_button();
		}
#endif //CONFIG_USE_ERROR_SERVO
		error_event_timer += 100;
	}
}
#endif