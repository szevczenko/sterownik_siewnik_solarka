#ifndef _ERROR_SOLARKA_H_
#define _ERROR_SOLARKA_H_
#include "config.h"

#if CONFIG_DEVICE_SOLARKA
#define ERROR_CRITICAL_VOLTAGE 800
#define ERROR_M_TIME_EXIT 1000

#define TEMP_WAIT_TO_TRY 30000

#define ACCUM_ERROR_VOLTAGE 9.0

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
	ERR_S_ERROR,
	ERR_S_EXIT
}err_temp_t;

void error_event(void);
uint16_t errorGetMotorVal(void);
void error_init(void);
void error_deinit(void);
void error_led_blink(void);

#endif //#if CONFIG_DEVICE_SIEWNIK

#endif



