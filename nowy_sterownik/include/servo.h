/*
 * pwm.h
 *
 * Created: 07.02.2019 13:19:21
 *  Author: Demetriusz
 */ 


#ifndef PWM_H_
#define PWM_H_
#include "tim.h"
#include "config.h"

//#define SERVO_PORT DDRD
//#define SERVO_PIN 
#define PWM_CLOSED 0
#define PWM_OPEN 100
#define TRY_OPEN_VAL 10
#define TIME_AFTER_RESET_SERVO_TRY 5000

#if CONFIG_DEVICE_SIEWNIK

#define CLOSE_SERVO servo_set_pwm_val(0);
#define OFF_SERVO set_pwm(19999);

typedef enum
{
	SERVO_NO_INIT = 0,
	SERVO_CLOSE,
	SERVO_OPEN,
	SERVO_DELAYED_OPEN,
	SERVO_TRY,
	SERVO_ERROR_PROCESS,
	SERVO_ERROR
}SERVOState;

typedef struct  
{
	uint8_t state;
	uint8_t last_state;
	uint8_t error_code;
	uint8_t value; // 0 - 100%
	evTime timeout;
	uint8_t try_cnt;
}sDriver;

void servo_init(uint8_t prescaler);
void servo_error(uint8_t close);
void servo_process(uint8_t value);
int servo_close(void);
void servo_enable_try(void);
int servo_open(uint8_t value);
int servo_is_open(void);
void servo_try_reset_timeout(uint32_t time_ms);
int servo_delayed_open(uint8_t value);
int servo_get_try_cnt(void);

#endif



#endif /* PWM_H_ */