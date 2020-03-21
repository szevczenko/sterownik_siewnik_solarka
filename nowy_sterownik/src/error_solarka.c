#include <stdint.h>
#include "measure.h"
#include "accumulator.h"
#include "error_solarka.h"
#include "dcmotorpwm.h"
#include "tim.h"
#include "system.h"
#include "buz.h"
#include "display_d.h"
#if CONFIG_DEVICE_SOLARKA
///////////////////////////////////////
///TODO get_calibration_value
int get_calibration_value(uint8_t type);
int get_disp_value(uint8_t type);
static void set_error_state(error_reason_ reason);
static uint16_t count_motor_error_value(uint16_t x);
static uint16_t count_motor_timeout_wait(uint16_t x);
static uint16_t count_motor_timeout_axelerate(uint16_t x);
static uint16_t count_temperature_error_value(void);

error_reason_ led_blink;

extern uint8_t servo_vibro_value;
//////////////////////////////////
//MOTOTR
extern uint8_t motor_value;
static evTime motor_timer;
static err_motor_t error_motor_state;
static err_motor_t error_motor_last_state;
static uint8_t error_motor_status;
static uint16_t motor_error_value;

//////////////////////////////////
//TEMPERATURE
static uint16_t temp_error_value;
static uint8_t error_temp_status;
static err_temp_t error_temp_state;
static evTime temp_timer;

void error_init(void)
{
	error_motor_state = ERR_M_OK;
	error_motor_last_state = ERR_M_OK;
	error_temp_state = ERR_S_OK;
	error_motor_status = 0;
	evTime_off(&motor_timer);
	evTime_off(&temp_timer);
	led_blink = ERR_REASON_NO;
	CLEAR_PIN(system_events, EV_SYSTEM_ERROR_MOTOR);
}

void error_deinit(void)
{
	error_motor_state = ERR_M_OK;
	error_motor_last_state = ERR_M_OK;
	error_motor_status = 0;
	evTime_off(&temp_timer);
	evTime_off(&motor_timer);
	led_blink = ERR_REASON_NO;
	error_temp_state = ERR_S_OK;
	CLEAR_PIN(system_events, EV_SYSTEM_ERROR_MOTOR);
}

uint16_t errorGetMotorVal(void)
{
	return motor_error_value;
}

void error_event(void)
{
	static uint32_t error_event_timer;
	if (error_event_timer < mktime.ms && system_events&(1<<EV_SYSTEM_STATE))
	{
		error_event_timer = mktime.ms + 500;
		if (system_events&(1<<EV_SYSTEM_ERROR_MOTOR)) return;
		///////////////////////////////////////////////////////////////////////////////////////////
		//MOTOR
		motor_error_value = count_motor_error_value(dcmotor_get_pwm());
		if (accum_get_voltage() < ACCUM_ERROR_VOLTAGE) //measure_get_filtered_value(MEAS_MOTOR) > motor_error_value
		{
			debug_msg("voltage_error: %f\n", accum_get_voltage());
			error_motor_status = 1;
		}
		else
		{
			error_motor_status = 0;
		}
		
		if (error_motor_status == 1)
		{
			switch(error_motor_state)
			{
				case ERR_M_OK:
					error_motor_state = ERR_M_WAIT;
					debug_msg("ERROR STATUS: ERR_M_WAIT\n\r");
					evTime_start(&motor_timer, 250); //count_motor_timeout_wait(dcmotor_get_pwm())
				break;
				case ERR_M_WAIT:
					if (evTime_check(&motor_timer))
					{
						dcmotor_set_try();
						evTime_start(&motor_timer, 250); //count_motor_timeout_axelerate(dcmotor_get_pwm())
						error_motor_state = ERR_M_AXELERATE;
						debug_msg("ERROR STATUS: ERR_M_AXELERATE\n\r");
					}
				break;
				case ERR_M_AXELERATE:
					if (evTime_check(&motor_timer))
					{
						error_motor_state = ERR_M_ERROR;
						debug_msg("ERROR STATUS: ERR_M_ERROR\n\r");
					}
				break;
				case ERR_M_ERROR:
					set_error_state(ERR_REASON_MOTOR);
				break;
				case ERR_M_EXIT:
					if (evTime_check(&motor_timer))
					{
						evTime_start(&motor_timer, ERROR_M_TIME_EXIT);
						error_motor_state = error_motor_last_state;
						debug_msg("ERROR STATUS: go to last before wait\n\r");
					}
				break;
			}
		}
		else
		{
			//toDo
			switch(error_motor_state)
			{
				case ERR_M_OK:
				break;
				case ERR_M_WAIT:
				if (evTime_check(&motor_timer))
				{
					evTime_start(&motor_timer, ERROR_M_TIME_EXIT);
					error_motor_state = ERR_M_EXIT;
					error_motor_last_state = ERR_M_WAIT;
					debug_msg("ERROR STATUS: ERR_M_EXIT\n\r");
				}
				break;
				case ERR_M_AXELERATE:
				if (evTime_check(&motor_timer))
				{
					evTime_start(&motor_timer, ERROR_M_TIME_EXIT);
					error_motor_state = ERR_M_EXIT;
					error_motor_last_state = ERR_M_AXELERATE;
					debug_msg("ERROR STATUS: ERR_M_EXIT\n\r");
				}
				break;
				case ERR_M_ERROR:
					set_error_state(ERR_REASON_MOTOR);
				break;
				case ERR_M_EXIT:
				if (evTime_check(&motor_timer))
				{
					debug_msg("ERROR STATUS: ERR_M_OK\n\r");
					error_motor_state = ERR_M_OK;
				}
				break;
			}
		}
		
		//////////////////////////////////////////////////////////////////////////////////////
		// TEMPERATURE
		
		temp_error_value = count_temperature_error_value();
		uint16_t measure_temp = measure_get_filtered_value(MEAS_TEMPERATURE);
		float volt = accum_get_voltage();
		debug_msg("TEMP ADC: %d, ADC_max: %d, voltage: %f\n", measure_temp, temp_error_value, volt);
		if (measure_get_filtered_value(MEAS_TEMPERATURE) > temp_error_value) //measure_get_filtered_value(MEAS_TEMPERATURE)
		{
			error_temp_status = 1;
		}
		else
		{
			error_temp_status = 0;
		}
		
		if (error_temp_status == 1)
		{
			switch(error_temp_state)
			{
				case ERR_S_OK:
				error_temp_state = ERR_S_WAIT;
				debug_msg("ERROR STATUS: przekroczenie TEMPERATURY. Timeout: %d [ms]\n\r", 30000);
				evTime_start(&temp_timer, TEMP_WAIT_TO_TRY);
				break;
				case ERR_S_WAIT:
				if (evTime_check(&temp_timer))
				{
					error_temp_state = ERR_S_ERROR;
					debug_msg("ERROR STATUS: ERR_T_ERROR\n\r");
				}
				break;
				case ERR_S_ERROR:
				set_error_state(ERR_REASON_TEMPERATURE);
				break;
			} //switch
		}// if (error_temp_status == 1)
		else
		{
			//toDo
			switch(error_temp_state)
			{
				case ERR_S_OK:
				break;
				case ERR_S_WAIT:
				if (evTime_check(&temp_timer))
				{
					error_temp_state = ERR_S_OK;
					debug_msg("ERROR STATUS: Temperatura jest OK. Normalny tryb.\n\r");
				}
				break;
				case ERR_S_ERROR:
				set_error_state(ERR_REASON_TEMPERATURE);
				break;
			} //switch
		} //else (error_temp_status == 1)
		
		
	} //error_event_timer
}


void error_led_blink(void)
{
	static evTime blink_timer;
	if (evTime_process_period(&blink_timer, 350))
	{
		if (led_blink == ERR_REASON_MOTOR)
		{
			LED_MOTOR_TOGGLE;
			ON_BUZZ_SIGNAL;
		}
		else if (led_blink == ERR_REASON_SERVO || led_blink == ERR_REASON_TEMPERATURE)
		{
			LED_SERVO_TOGGLE;
			ON_BUZZ_SIGNAL;
		}
	}
}

static void set_error_state(error_reason_ reason)
{
	debug_msg("VIBRO: error %d\n",reason);
	SET_PIN(system_events, EV_SYSTEM_ERROR_MOTOR);
	display_set_error(reason);
	dcmotor_set_error();
	system_error();
	led_blink = reason;
}

int get_calibration_value(uint8_t type)
{
	return 100;
}


static uint16_t count_motor_error_value(uint16_t x)
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

static uint16_t count_motor_timeout_wait(uint16_t x)
{
	uint16_t timeout = 5000 - x*30;
	debug_msg("count_motor_timeout_wait: %d\n\r", timeout);
	return timeout; //5000[ms] - pwm*30
}

static uint16_t count_motor_timeout_axelerate(uint16_t x)
{
	uint16_t timeout = 5000 - x*30;
	debug_msg("count_motor_timeout_axelerate: %d\n\r", timeout);
	return timeout; //5000[ms] - pwm*30
}

static uint16_t count_temperature_error_value(void)
{
	return (uint16_t)(47.62*accum_get_voltage() - 95);
}

#endif