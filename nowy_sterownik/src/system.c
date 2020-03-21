#include "config.h"
#include "display_d.h"
#include "dcmotorpwm.h"
#include "mem.h"
#include "vibro.h"
#include "servo.h"
#include "buz.h"
#include "accumulator.h"
#include "measure.h"
#include "system.h"
#include "but.h"
#include "mem.h"
#include "buz.h"
#include "error_siewnik.h"
#include "error_solarka.h"

static void on_button(void);
static void off_button(void);

extern uint8_t servo_vibro_value;
static uint8_t counter_very_fast_servo_add;
static uint8_t counter_very_fast_servo_sub;

extern uint8_t motor_value;
static uint8_t counter_very_fast_motor_add;
static uint8_t counter_very_fast_motor_sub;

static uint8_t state_measure_volt = 0;


////////////////////////////////////////////////////////////
///// OBSLUGA DODAWANIA ODEJMOWANIA

void b_servo_vibro_add_fall (void * pv);
void b_servo_vibro_sub_fall(void * pv);
void b_servo_vibro_add_rise (void * pv);
void b_servo_vibro_sub_rise(void * pv);
void b_servo_vibro_add_timer(void * pv);
void b_servo_vibro_sub_timer(void * pv);

void button_motor_add_rise_cb (void * pv);
void button_motor_sub_rise_cb(void * pv);
void button_motor_add_fall_cb (void * pv);
void button_motor_sub_fall_cb(void * pv);
void button_motor_add_timer_cb(void * pv);
void button_motor_sub_timer_cb(void * pv);

///////////////////////////////////////////////////////////

void button_dcmotor_cb(void *pv);
#if CONFIG_DEVICE_SOLARKA
void button_fall_solar(void * pv);
void button_rise_solar(void * pv);
void button_timer_solar(void * pv);
#endif
#if CONFIG_DEVICE_SIEWNIK
void button_on_off_pwm(void *pv);
#endif
void button4_toggle_system(void *pv);

void button8_prg_rise_cb(void *pv);
void button8_prg_fall_cb(void *pv);
void button8_prg_save_cb(void *pv);

void button9_prg_rise_cb(void *pv);
void button9_prg_fall_cb(void *pv);
void button9_prg_save_cb(void *pv);

void button10_prg_rise_cb(void *pv);
void button10_prg_fall_cb(void *pv);
void button10_prg_save_cb(void *pv);

mem_program mem_last_save;
mem_program mem_save_prog;

static void sys_read_display(void)
{
	mem_read_data(MEM_LAST_DATA, &mem_last_save);
	if (mem_last_save.second_disp_val >=10 && mem_last_save.second_disp_val < 100)
		servo_vibro_value = mem_last_save.second_disp_val;
	if (mem_last_save.first_disp_val >=10 && mem_last_save.first_disp_val < 100)
		motor_value = mem_last_save.first_disp_val;
}

static void sys_save_display(void)
{
	mem_save_prog.first_disp_val = motor_value;
	mem_save_prog.second_disp_val = servo_vibro_value;
	mem_save_data(MEM_LAST_DATA, &mem_save_prog);
}

void init_system(void)
{
	button4.rise_callback = button4_toggle_system;
	disp_set_state(DISP_ALL,SEG_OFF);
	sys_read_display();
}


void on_system(void)
{
	state_measure_volt = 0;
	on_animation(ANIMATION_TIME_MS, DISP_ALL);
	#if DARK_MENU
	
	#endif
	#if CONFIG_DEVICE_SOLARKA
	vibro_init();
	#endif
	dcmotorpwm_init();
	#if CONFIG_DEVICE_SIEWNIK
	servo_init(0);
	#endif
	error_init();
	init_measure();
	init_buzzer();
	system_events = 0;
	SET_PIN(system_events, EV_SYSTEM_STATE);
	on_button();
	#if DARK_MENU
	#endif
	dip_set_value(DISP_MOTOR, motor_value);
	dip_set_value(DISP_SERVO, servo_vibro_value);
	//debug_msg("SYS: On system\n");
}

void off_system(void)
{
	state_measure_volt = 0;
	dcmotorpwm_deinit();
	off_button();
	error_deinit();
	#if CONFIG_DEVICE_SIEWNIK
	servo_close();
	#endif
	#if CONFIG_DEVICE_SOLARKA
	vibro_deInit();
	#endif
	system_events = 0;
	disp_set_state(DISP_ALL,SEG_OFF);
	CLEAR_PIN(system_events, EV_SYSTEM_STATE);
	//debug_msg("SYS: Off system %d\n", system_events);
}


void off_button(void)
{
	button1.timer_callback = 0;
	button1.rise_callback = 0;
	button1.fall_callback = 0;
	
	button2.timer_callback = 0;
	button2.rise_callback = 0;
	button2.fall_callback = 0;
	
	button3.fall_callback = 0;
	
	button5.timer_callback = 0;
	button5.fall_callback = 0;
	button5.rise_callback = 0;
	
	button6.timer_callback = 0;
	button6.fall_callback = 0;
	button6.rise_callback = 0;
	
	button7.fall_callback = 0;
	
	button8.fall_callback = 0;
	button8.timer_callback = 0;
	button8.rise_callback = 0;
	
	button9.fall_callback = 0;
	button9.timer_callback = 0;
	button9.rise_callback = 0;
	
	button10.fall_callback = 0;
	button10.timer_callback = 0;
	button10.rise_callback = 0;
}


static void on_button(void)
{
 	button1.timer_callback = button_motor_sub_timer_cb;
 	button1.fall_callback = button_motor_sub_fall_cb;
 	button1.rise_callback = button_motor_sub_rise_cb;
// 	
	button2.timer_callback = button_motor_add_timer_cb;
	button2.fall_callback = button_motor_add_fall_cb;
	button2.rise_callback = button_motor_add_rise_cb;
// 	
 	button3.fall_callback = button_dcmotor_cb;
// 		
	button5.timer_callback = b_servo_vibro_sub_timer;
	button5.rise_callback = b_servo_vibro_sub_rise;
	button5.fall_callback = b_servo_vibro_sub_fall;
	
	button6.timer_callback = b_servo_vibro_add_timer;
	button6.rise_callback = b_servo_vibro_add_rise;
	button6.fall_callback = b_servo_vibro_add_fall;
// 	
	#if CONFIG_DEVICE_SIEWNIK
 	button7.fall_callback = button_on_off_pwm;
	#endif
	#if CONFIG_DEVICE_SOLARKA
	button7.fall_callback = button_fall_solar;
	button7.rise_callback= button_rise_solar;
	button7.timer_callback = button_timer_solar;
	#endif
}

/////////////////////////////////////////
//MOTOR SUB

void button_motor_sub_timer_cb(void * pv)
{
	if (state_measure_volt != 2)
		SET_PIN(system_events, EV_MOTOR_DISP_FAST_SUB);
}

void button_motor_sub_fall_cb(void * pv)
{
	ON_BUZZ_SIGNAL;
	if (motor_value > 10)
		motor_value--;
	dip_set_value(DISP_MOTOR, motor_value);
	if (disp_get_state(DISP_MOTOR) == SEG_ANIMATION)
		disp_set_state(DISP_ALL, SEG_VALUE);
	else disp_set_state(DISP_MOTOR, SEG_VALUE);
	state_measure_volt++;
	if (state_measure_volt == 2)
	{
		disp_set_state(DISP_ALL, SEG_VOLTAGE);
	}
}

void button_motor_sub_rise_cb(void * pv)
{
	disp_disable_voltage();
	CLEAR_PIN(system_events, EV_MOTOR_DISP_FAST_SUB);
	CLEAR_PIN(system_events, EV_MOTOR_DISP_VERY_FAST_SUB);
	counter_very_fast_motor_sub = 0;
	if (state_measure_volt != 0)
		state_measure_volt--;
	if (disp_get_state(DISP_MOTOR) == SEG_ANIMATION)
	disp_set_state(DISP_ALL, SEG_VALUE);
	else disp_set_state(DISP_MOTOR, SEG_VALUE);
}

/////////////////////////////////////////
//MOTOR ADD
void button_motor_add_timer_cb(void * pv)
{
	if (state_measure_volt != 2)
		SET_PIN(system_events, EV_MOTOR_DISP_FAST_ADD);
}

void button_motor_add_fall_cb(void * pv)
{
	ON_BUZZ_SIGNAL;
	if (motor_value < 99)
		motor_value++;
	dip_set_value(DISP_MOTOR, motor_value);
	if (disp_get_state(DISP_MOTOR) == SEG_ANIMATION)
	disp_set_state(DISP_ALL, SEG_VALUE);
	else disp_set_state(DISP_MOTOR, SEG_VALUE);
	state_measure_volt++;
	if (state_measure_volt == 2)
	{
		disp_set_state(DISP_ALL, SEG_VOLTAGE);
	}
}

void button_motor_add_rise_cb(void * pv)
{
	disp_disable_voltage();
	CLEAR_PIN(system_events, EV_MOTOR_DISP_FAST_ADD);
	CLEAR_PIN(system_events, EV_MOTOR_DISP_VERY_FAST_ADD);
	counter_very_fast_motor_sub = 0;
	if (state_measure_volt != 0)
		state_measure_volt--;
	if (disp_get_state(DISP_MOTOR) == SEG_ANIMATION)
	disp_set_state(DISP_ALL, SEG_VALUE);
	else disp_set_state(DISP_MOTOR, SEG_VALUE);
}

/////////////////////////////////////////
//SERVO VIBRO SUB
void b_servo_vibro_sub_timer(void * pv)
{
	SET_PIN(system_events, EV_SERVO_DISP_FAST_SUB);
}

void b_servo_vibro_sub_fall(void * pv)
{
	ON_BUZZ_SIGNAL;
	#if CONFIG_VIBRO_TIME_ON
	if (servo_vibro_value > 0)
	servo_vibro_value--;
	if (vibro_get_state() == VIBRO_ENABLE_TIME || vibro_get_state() == VIBRO_ON_PERIOD)
	{
		vibro_change_period(servo_vibro_value * 100);
	}
	else
	{
		dip_set_value(DISP_SERVO, servo_vibro_value);
		disp_set_state(DISP_ALL, SEG_VALUE);
	}
	
	#else
	if (servo_vibro_value > 10)
	servo_vibro_value--;
	dip_set_value(DISP_SERVO, servo_vibro_value);
	disp_set_state(DISP_ALL, SEG_VALUE);
	#endif
}

void b_servo_vibro_sub_rise(void * pv)
{
	CLEAR_PIN(system_events, EV_SERVO_DISP_FAST_SUB);
	CLEAR_PIN(system_events, EV_SERVO_DISP_VERY_FAST_SUB);
	counter_very_fast_motor_sub = 0;
}

/////////////////////////////////////////
//SERVO VIBRO ADD
void b_servo_vibro_add_timer(void * pv)
{
	SET_PIN(system_events, EV_SERVO_DISP_FAST_ADD);
}

void b_servo_vibro_add_fall(void * pv)
{
	ON_BUZZ_SIGNAL;
	#if CONFIG_VIBRO_TIME_ON
	if (servo_vibro_value < 99)
	servo_vibro_value++;
	if (vibro_get_state() == VIBRO_ENABLE_TIME || vibro_get_state() == VIBRO_ON_PERIOD)
	{
		vibro_change_period(servo_vibro_value * 100);
	}
	else
	{
		dip_set_value(DISP_SERVO, servo_vibro_value);
		disp_set_state(DISP_ALL, SEG_VALUE);
	}
	
	#else
	if (servo_vibro_value < 99)
	servo_vibro_value++;
	dip_set_value(DISP_SERVO, servo_vibro_value);
	disp_set_state(DISP_ALL, SEG_VALUE);
	#endif
	
}

void b_servo_vibro_add_rise(void * pv)
{
	CLEAR_PIN(system_events, EV_SERVO_DISP_FAST_ADD);
	CLEAR_PIN(system_events, EV_SERVO_DISP_VERY_FAST_ADD);
	counter_very_fast_motor_sub = 0;
}


void button_dcmotor_cb(void * pv)
{
	ON_BUZZ_SIGNAL;
	if (motor_value < 10)
	{
		motor_value = 10;
		dip_set_value(DISP_MOTOR, motor_value);
	}
	sys_save_display();
	if(!dcmotor_is_on())
	{ 
		dcmotorpwm_start();
		#if CONFIG_DEVICE_SIEWNIK
		servo_delayed_open(servo_vibro_value);
		#endif
	}
	else 
	{
		dcmotorpwm_stop();
		#if CONFIG_DEVICE_SIEWNIK
		servo_close();
		#endif
	}
}
#if CONFIG_DEVICE_SIEWNIK
void button_on_off_pwm(void * pv)
{
	ON_BUZZ_SIGNAL;
	if (servo_vibro_value < 10)
	{
		servo_vibro_value = 10;
		dip_set_value(DISP_SERVO, servo_vibro_value);
	}
	
	if (servo_is_open()) servo_close();
	else servo_open(servo_vibro_value);
}
#endif

#if CONFIG_DEVICE_SOLARKA

void button_fall_solar(void * pv)
{
	dip_set_value(DISP_MOTOR, motor_value);
	disp_set_state(DISP_MOTOR, SEG_VALUE);
	ON_BUZZ_SIGNAL;
	#if CONFIG_VIBRO_TIME_ON
	if (vibro_get_state() != VIBRO_ENABLE_TIME && vibro_get_state() != VIBRO_ON_PERIOD)
	{
		dip_set_value(DISP_SERVO, servo_vibro_value);
		disp_set_state(DISP_SERVO, SEG_VALUE);
		vibro_on_period(servo_vibro_value * 100);
		vibro_set_period(servo_vibro_value * 100);
	}
	else
	{
		vibro_disable_time_on();
		dip_set_value(DISP_SERVO, servo_vibro_value);
	}
	
	#else
	
	if (servo_vibro_value < 10)
	{
		servo_vibro_value = 10;
	}
	//////////// OPCJONALNE
// 	if (disp_get_state(DISP_SERVO) == SEG_BLINK)
// 	{
// 		disp_set_state(DISP_SERVO, SEG_VALUE);
// 		dip_set_value(DISP_SERVO, servo_vibro_value);
// 		return;
// 	}
	//////////////
	if (vibro_get_state() != VIBRO_ON_TIMEOUT)
	{
		dip_set_value(DISP_SERVO, servo_vibro_value);
		disp_set_state(DISP_SERVO, SEG_VALUE);
		vibro_on_time(servo_vibro_value * 100);
	}
	else
	{
		vibro_off_time_disp();
		dip_set_value(DISP_SERVO, servo_vibro_value);
	}	
	#endif
}
#include <string.h>
void button_rise_solar(void * pv)
{
	sys_save_display();
	if(vibro_is_on())
	{
		vibro_off();
		return;
	}
	else if (vibro_get_state() != VIBRO_OFF)
	{
		#if CONFIG_VIBRO_TIME_ON
		vibro_on_time_timeout(servo_vibro_value * 100);
		vibro_set_period(servo_vibro_value * 100);
		#else
		vibro_on_time_disp(servo_vibro_value * 100);
		#endif
	}
}

void button_timer_solar(void * pv)
{
	vibro_on();
}
#endif

void button4_toggle_system(void *pv)
{
	sys_save_display();
	if (system_events&(1<<EV_SYSTEM_STATE))
	{
		off_system();
	}
	else
	{
		on_system();
	}
	ON_BUZZ_SIGNAL;
}



void disp_fast_process(void)
{
	static timer_t disp_fast_timer;
	if (disp_fast_timer < mktime.ms)
	{
		if (system_events&(1<<EV_SERVO_DISP_FAST_ADD) && servo_vibro_value < 99)
		{
			///////////////////////////////////////
			/// opoznienie eroru serwa
			#if CONFIG_VIBRO_TIME_ON
			servo_vibro_value++;
			if (vibro_get_state() == VIBRO_ENABLE_TIME || vibro_get_state() == VIBRO_ON_PERIOD)
			{
				vibro_change_period(servo_vibro_value * 100);
			}
			else
			{
				dip_set_value(DISP_SERVO, servo_vibro_value);
				disp_set_state(DISP_ALL, SEG_VALUE);
			}
			
			#else
			dip_set_value(DISP_SERVO, ++servo_vibro_value);
			#endif
			counter_very_fast_servo_add++;
			if (counter_very_fast_servo_add >= VERY_FAST_COUNTER_OVF)
			{
				SET_PIN(system_events, EV_SERVO_DISP_VERY_FAST_ADD);
				CLEAR_PIN(system_events, EV_SERVO_DISP_FAST_ADD);
				counter_very_fast_servo_add = 0;
			}
		}
		if (system_events&(1<<EV_SERVO_DISP_FAST_SUB) && servo_vibro_value > 0)
		{
			///////////////////////////////////////
			/// opoznienie eroru serwa
			//ON_BUZZ_SIGNAL;
			#if CONFIG_VIBRO_TIME_ON
			servo_vibro_value--;
			if (vibro_get_state() == VIBRO_ENABLE_TIME || vibro_get_state() == VIBRO_ON_PERIOD)
			{
				vibro_change_period(servo_vibro_value * 100);
			}
			else
			{
				dip_set_value(DISP_SERVO, servo_vibro_value);
				disp_set_state(DISP_ALL, SEG_VALUE);
			}
			
			#else
			dip_set_value(DISP_SERVO, --servo_vibro_value);
			#endif
			counter_very_fast_servo_sub++;
			if (counter_very_fast_servo_sub >= VERY_FAST_COUNTER_OVF)
			{
				SET_PIN(system_events, EV_SERVO_DISP_VERY_FAST_SUB);
				CLEAR_PIN(system_events, EV_SERVO_DISP_FAST_SUB);
				counter_very_fast_servo_sub = 0;
			}
		}
		
		if (system_events&(1<<EV_MOTOR_DISP_FAST_ADD) && motor_value < 99)
		{
			///////////////////////////////////////
			/// opoznienie eroru serwa
			dip_set_value(DISP_MOTOR, ++motor_value);
			counter_very_fast_motor_add++;
			if (counter_very_fast_motor_add >= VERY_FAST_COUNTER_OVF)
			{
				SET_PIN(system_events, EV_MOTOR_DISP_VERY_FAST_ADD);
				CLEAR_PIN(system_events, EV_MOTOR_DISP_FAST_ADD);
				counter_very_fast_motor_add = 0;
			}
		}
		if (system_events&(1<<EV_MOTOR_DISP_FAST_SUB) && motor_value > 10)
		{
			///////////////////////////////////////
			/// opoznienie eroru serwa
			dip_set_value(DISP_MOTOR, --motor_value);
			counter_very_fast_motor_sub++;
			if (counter_very_fast_motor_sub >= VERY_FAST_COUNTER_OVF)
			{
				SET_PIN(system_events, EV_MOTOR_DISP_VERY_FAST_SUB);
				CLEAR_PIN(system_events, EV_MOTOR_DISP_FAST_SUB);
				counter_very_fast_motor_sub = 0;
			}
		}
		disp_fast_timer += 80;
	}
}

static timer_t disp_very_fast_timer;

void disp_very_fast_process(void)
{
	if (disp_very_fast_timer < mktime.ms)
	{
		if (servo_vibro_value < 99 && system_events&(1<<EV_SERVO_DISP_VERY_FAST_ADD))
		{
			#if CONFIG_VIBRO_TIME_ON
			servo_vibro_value++;
			if (vibro_get_state() == VIBRO_ENABLE_TIME || vibro_get_state() == VIBRO_ON_PERIOD)
			{
				vibro_change_period(servo_vibro_value * 100);
			}
			else
			{
				dip_set_value(DISP_SERVO, servo_vibro_value);
				disp_set_state(DISP_ALL, SEG_VALUE);
			}
			
			#else
			dip_set_value(DISP_SERVO, ++servo_vibro_value);
			#endif
		}
		if (servo_vibro_value > 0 && system_events&(1<<EV_SERVO_DISP_VERY_FAST_SUB))
		{
			#if CONFIG_VIBRO_TIME_ON
			servo_vibro_value--;
			if (vibro_get_state() == VIBRO_ENABLE_TIME || vibro_get_state() == VIBRO_ON_PERIOD)
			{
				vibro_change_period(servo_vibro_value * 100);
			}
			else
			{
				dip_set_value(DISP_SERVO, servo_vibro_value);
				disp_set_state(DISP_ALL, SEG_VALUE);
			}
			
			#else
			dip_set_value(DISP_SERVO, --servo_vibro_value);
			#endif
		}
		
		if (motor_value < 99 && system_events&(1<<EV_MOTOR_DISP_VERY_FAST_ADD))
		{
			//ON_BUZZ_SIGNAL;
			dip_set_value(DISP_MOTOR,++motor_value);
		}
		if (motor_value > 0 && system_events&(1<<EV_MOTOR_DISP_VERY_FAST_SUB))
		{
			//ON_BUZZ_SIGNAL;
			if (motor_value > 10)
			dip_set_value(DISP_MOTOR,--motor_value);
		}
		disp_very_fast_timer += 30;
	}
}


void system_error(void)
{
	off_button();
	#if CONFIG_DEVICE_SOLARKA
	vibro_off();
	#endif
}
