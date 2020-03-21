#ifndef _ERROR_H_
#define _ERROR_H_
#include "config.h"

#if CONFIG_DEVICE_SIEWNIK
#define ERROR_CRITICAL_VOLTAGE 800
#define ERROR_M_TIME_EXIT 2000

#define SERVO_WAIT_TO_TRY 500
#define SERVO_WAIT_AFTER_TRY 1500
#define SERVO_TRY_CNT 3

typedef enum
{
	ERR_M_OK,
	ERR_M_WAIT,
	ERR_M_AXELERATE,
	ERR_M_ERROR,
	ERR_M_EXIT
}err_motor_t;

typedef enum
{
	ERR_S_OK,
	ERR_S_WAIT,
	ERR_S_TRY,
	ERR_S_ERROR,
	ERR_S_EXIT
}err_servo_t;

void error_event(void);
uint16_t errorGetMotorVal(void);
void error_init(void);
void error_deinit(void);

#endif //#if CONFIG_DEVICE_SIEWNIK

#endif



