/*
 * nowy_sterownik.c
 *
 * Created: 24.09.2019 18:07:05
 * Author : Demetriusz
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "system.h"
#include "config.h"
#include "display_d.h"
#include "but.h"
#include "usart.h"
#include "measure.h"
#include "accumulator.h"
#include "servo.h"
#include "dcmotorpwm.h"
#include "vibro.h"
#include "buz.h"
#include "error_siewnik.h"
#include "error_solarka.h"
#include "dark_menu.h"

#if L_DEBUG
#include <stdio.h>
char debug_buff[64];
void debug_msg( const char* format, ... )
{
	va_list arglist;
	va_start( arglist, format );
	vsprintf(debug_buff, format, arglist );
	va_end( arglist );
	#if USE_USART
	uart_puts(debug_buff);
	#endif
}
#endif

#if SERIAL_PLOT
void uart_process(void)
{
	static timer_t uart_send_timer;
	static uint8_t *pnt;
	static uint16_t send_value;
	if (uart_send_timer < mktime.ms)
	{
		pnt = &send_value;
		send_value = (uint16_t)errorGetMotorVal();
		uart_putc(255);
		uart_putc(pnt[0]);
		uart_putc(pnt[1]);
		send_value = measure_get_filtered_value(MEAS_MOTOR);
		uart_putc(pnt[0]);
		uart_putc(pnt[1]);
		uart_send_timer += 100;
		
	}
}
#endif



uint16_t system_events;
uint8_t motor_value;
uint8_t servo_vibro_value;

void init_pin(void)
{
	SET_PIN(LED_MOTOR_DDR, LED_MOTOR_PIN);
	SET_PIN(LED_SERVO_DDR, LED_SERVO_PIN);
	SET_PIN(LED_ACUM_DDR, LED_ACUM_PIN);
	//UART_TX_PIN
	DDRD |= (1<<1);
}

void init_driver(void)
{
	#if USE_USART
	uart_init(UART_BAUD_SELECT(57600, F_CPU));
	#endif
	timer0_init(TIM0_PRESCALER, TIM0_ARR);
	init_display_drv();
	init_buttons();
	init_system();
	init_measure();
	CLEAR_PIN(SFIOR, PUD);
}

int main(void)
{
	#if CONFIG_DEVICE_SIEWNIK
	servo_init(0);
	#endif
	init_pin();
	sei();
	init_driver();
	debug_msg("/-----------START SYSTEM------------/\n");
	#if CONFIG_DEVICE_SIEWNIK
	init_menu();
	#endif
	#if CONFIG_DEVICE_SIEWNIK
	measure_get_servo_calibration();
	#endif
    /* Replace with your application code */
    while (1) 
    {
		disp_process();
		animation();
		process_button();
		disp_fast_process();
		disp_very_fast_process();
		dcmotor_process(motor_value);
		#if CONFIG_DEVICE_SIEWNIK
		servo_process(servo_vibro_value);
		menu_process();
		#endif
		#if CONFIG_DEVICE_SOLARKA
		vibro_process();
		#endif
		error_event();
		error_led_blink();
		measure_process();
		accumulator_process();
		accum_led_process();
		buzzer_process();
		#if SERIAL_PLOT && USE_USART
		uart_process();
		#endif
    }
}

