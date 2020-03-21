/*
 * but.c
 *
 * Created: 05.02.2019 17:20:37
 *  Author: Demetriusz
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "disp.h"
#include "tim.h"
#include "stdint.h"
#include "but.h"


extern uint8_t test_number;

uint32_t timer_button;

but_t button1, button2, button3, button4, button5, button6, button7, button8, button9, button10;
but_t *but_tab[BUTTON_CNT] = {&button1, &button2, &button3, &button4, &button5, &button6, &button7, &button8, &button9, &button10};
extern time_t mktime;

uint8_t read_button(but_t *but)
{
	if (*but->port  & (1<<but->pin))
		return 1;
	else
		return 0;
}

extern uint8_t test_button;
void test_fnc(void * pv)
{
}

void init_but_struct(void)
{
	
	button1.state = 0;
	button1.value = 1;
	button1.fall_callback = 0;
	button1.rise_callback = 0;
	button1.timer_callback = 0;//test_fnc;
	button1.port = &BUT1_RPORT;
	button1.pin = BUT1_PIN;
	
	button2.state = 0;
	button2.value = 1;
	button2.fall_callback = 0;
	button2.rise_callback = 0;
	button2.timer_callback = 0;
	button2.port = &BUT2_RPORT;
	button2.pin = BUT2_PIN;
	
	button3.state = 0;
	button3.value = 1;
	button3.fall_callback = 0;
	button3.rise_callback = 0;
	button3.timer_callback = 0;
	button3.port = &BUT3_RPORT;
	button3.pin = BUT3_PIN;
	
	button4.state = 0;
	button4.value = 1;
	button4.fall_callback = 0;
	button4.rise_callback = 0;
	button4.timer_callback = 0;
	button4.port = &BUT4_RPORT;
	button4.pin = BUT4_PIN;
	
	button5.state = 0;
	button5.value = 1;
	button5.fall_callback = 0;
	button5.rise_callback = 0;
	button5.timer_callback = 0;
	button5.port = &BUT5_RPORT;
	button5.pin = BUT5_PIN;
	
	button6.state = 0;
	button6.value = 1;
	button6.fall_callback = 0;
	button6.rise_callback = 0;
	button6.timer_callback = 0;
	button6.port = &BUT6_RPORT;
	button6.pin = BUT6_PIN;
	
	button7.state = 0;
	button7.value = 1;
	button7.fall_callback = 0;
	button7.rise_callback = 0;
	button7.timer_callback = 0;
	button7.port = &BUT7_RPORT;
	button7.pin = BUT7_PIN;
	
	button8.state = 0;
	button8.value = 1;
	button8.fall_callback = 0;
	button8.rise_callback = 0;
	button8.timer_callback = 0;
	button8.port = &BUT8_RPORT;
	button8.pin = BUT8_PIN;
	
	button9.state = 0;
	button9.value = 1;
	button9.fall_callback = 0;
	button9.rise_callback = 0;
	button9.timer_callback = 0;
	button9.port = &BUT9_RPORT;
	button9.pin = BUT9_PIN;
	
	button10.state = 0;
	button10.value = 1;
	button10.fall_callback = 0;
	button10.rise_callback = 0;
	button10.timer_callback = 0;
	button10.port = &BUT10_RPORT;
	button10.pin = BUT10_PIN;
}

void process_button(void)
{
	uint8_t red_val = 0;
	if (timer_button < mktime.ms)
	{
		timer_button = mktime.ms + 20;
		//process
		for (uint8_t i=0; i<BUTTON_CNT; i++)
		{
			red_val = read_button(but_tab[i]);
			if(red_val != but_tab[i]->value)
			{
				but_tab[i]->value = red_val;
				if (red_val == 1 && but_tab[i]->rise_callback != 0)
				but_tab[i]->rise_callback(but_tab[i]);
				else if(red_val == 0 && but_tab[i]->fall_callback != 0)
				but_tab[i]->fall_callback(but_tab[i]);
			}
			//timer
			if (red_val == 0)
			{
				but_tab[i]->tim_cnt++;
				if (but_tab[i]->tim_cnt >=TIMER_CNT_TIMEOUT && but_tab[i]->state != 1)
				{
					if (but_tab[i]->timer_callback != 0)
					but_tab[i]->timer_callback(&button1);
					but_tab[i]->tim_cnt = 0;
					but_tab[i]->state = 1;
				}
			}
			else
			{
				but_tab[i]->tim_cnt = 0;
				but_tab[i]->state = 0;
			}
		} // end for
	}// end if
}

void init_buttons(void)
{
	init_but_struct();
	//button1
	CLEAR_PIN(BUT1_DDR, BUT1_PIN);
	SET_PIN(BUT1_PORT, BUT1_PIN);
	
	CLEAR_PIN(BUT2_DDR, BUT2_PIN);
	SET_PIN(BUT2_PORT, BUT2_PIN);
	
	CLEAR_PIN(BUT3_DDR, BUT3_PIN);
	SET_PIN(BUT3_PORT, BUT3_PIN);
	
	CLEAR_PIN(BUT4_DDR, BUT4_PIN);
	SET_PIN(BUT4_PORT, BUT4_PIN);
	
	CLEAR_PIN(BUT5_DDR, BUT5_PIN);
	SET_PIN(BUT5_PORT, BUT5_PIN);
	
	CLEAR_PIN(BUT6_DDR, BUT6_PIN);
	SET_PIN(BUT6_PORT, BUT6_PIN);
	
	CLEAR_PIN(BUT7_DDR, BUT7_PIN);
	SET_PIN(BUT7_PORT, BUT7_PIN);
	
	CLEAR_PIN(BUT8_DDR, BUT8_PIN);
	SET_PIN(BUT8_PORT, BUT8_PIN);
	
	CLEAR_PIN(BUT9_DDR, BUT9_PIN);
	SET_PIN(BUT9_PORT, BUT9_PIN);
	
	CLEAR_PIN(BUT10_DDR, BUT10_PIN);
	SET_PIN(BUT10_PORT, BUT10_PIN);

}

/*
		red_val = read_button(&button1);
		if(red_val != button1.value) 
		{
			button1.value = red_val;
			if (red_val == 1 && button1.rise_callback != 0)
				button1.rise_callback(&button1);
			else if(red_val == 0 && button1.fall_callback != 0)
				button1.fall_callback(&button1);
		}
		//timer
		if (red_val == 0)
		{
			button1.tim_cnt++;
			if (button1.tim_cnt >=50 && button1.state != 1)
			{
				if (button1.timer_callback != 0)
					button1.timer_callback(&button1);
				button1.tim_cnt = 0;
				button1.state = 1;
			}
		}
		else 
		{
			button1.tim_cnt = 0;
			button1.state = 0;
		}
		*/