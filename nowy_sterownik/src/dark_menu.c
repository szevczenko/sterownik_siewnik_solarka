/*
 * dark_menu.c
 *
 * Created: 13.03.2019 15:25:21
 *  Author: Demetriusz
 */ 
#include <config.h>
#if DARK_MENU
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "tim.h"
#include "dark_menu.h"
#include "eeprom.h"
#include "disp.h"
#include "but.h"

static uint8_t last_button;
uint8_t state_menu;
menu_t menu_param;
#define MENU_ADDR 12

static void save_parametrs(void)
{
	uint8_t* pnt = (uint8_t*) &menu_param;
	for (uint8_t byte_n=MENU_ADDR; byte_n<MENU_ADDR+sizeof(menu_t);byte_n++)
	{
		EEPROM_write(byte_n,*pnt);
		pnt++;
	}
}

void read_menu_parametrs(void)
{
	uint8_t* pnt = (uint8_t*) &menu_param;
	for (uint8_t byte_n=MENU_ADDR; byte_n<MENU_ADDR+sizeof(menu_t);byte_n++)
	{
		*pnt = EEPROM_read(byte_n);
		pnt++;
	}
}

void button_1_menu(void *pv)
{
	if (last_button == 10 && state_menu == 0) last_button = 8;
	else if(state_menu == 0) last_button = 0;
	
	if (state_menu == 1 && menu_param.motor_add > 1)
	{
		menu_param.motor_add--;
	}
}
void button_2_menu(void *pv)
{
	if (last_button == 0 && state_menu == 0) last_button = 9;
	else if(state_menu == 0) last_button = 0;
	
	if (state_menu == 1 && menu_param.motor_add < 99)
	{
		menu_param.motor_add++;
	}
}
void button_3_menu(void *pv)
{
	if (last_button == 9 && state_menu == 0) last_button = 10;
	else if(state_menu == 0) last_button = 0;
	
	if (state_menu == 1 && menu_param.motor_add <= 99 && menu_param.servo_add <= 99)
	{
		save_parametrs();
		segment1.state = SEG_MENU_SAVE; 
		segment2.state = SEG_MENU_SAVE;
		LED_MOTOR_OFF;
		LED_SERVO_OFF;
	}
}

void button_5_menu(void *pv)
{
	if (state_menu == 1 && menu_param.servo_add > 1)
	{
		menu_param.servo_add--;
	}
	
	if (last_button == 7 && state_menu == 0)
	{ 
		state_menu = 1;
		last_button = 0;
		if (menu_param.disable_error == 1) LED_SERVO_OFF;
		else LED_SERVO_ON;
		LED_MOTOR_ON;
		segment1.state = SEG_MENU;
		segment2.state = SEG_MENU;
	}
	else if(state_menu == 0) last_button = 0;
}
void button_6_menu(void *pv)
{
	if (last_button == 8 && state_menu == 0) last_button = 6;
	else if(state_menu == 0) last_button = 0;
	if (state_menu == 1 && menu_param.servo_add < 99)
	{
		menu_param.servo_add++;
	}
}
void button_7_menu(void *pv)
{
	if (last_button == 6 && state_menu == 0) last_button = 7;
	else if(state_menu == 0) last_button = 0;
	
	if (state_menu == 1 && menu_param.disable_error == 0)
	{
		menu_param.disable_error = 1;
		LED_SERVO_OFF;
	}
	else if (state_menu == 1 && menu_param.disable_error == 1)
	{
		menu_param.disable_error = 0;
		LED_SERVO_ON;
	}
}
void button_8_menu(void *pv)
{
	
}
void button_9_menu(void *pv)
{
	
}
void button_10_menu(void *pv)
{
	
}

void menu_process(void)
{
	static timer_t menu_timer;
	if (menu_timer < mktime.ms && state_menu == 1)
	{
		if (segment1.state == SEG_MENU)
			disp_set_number(&segment1, menu_param.motor_add);
		if (segment2.state == SEG_MENU)
			disp_set_number(&segment2, menu_param.servo_add);
			
		if (segment1.state == SEG_MENU_SAVE && segment2.state == SEG_MENU_SAVE)
		{
			menu_timer = 1000 + mktime.ms;
			DispD2.value = 's';
			DispD1.value = 'a';
			DispD3.value = 'p';
			DispD4.value = 0;
			segment1.state = SEG_MENU_OFF;
			segment2.state = SEG_MENU_OFF;
			
			return;
		}
		
		if (segment1.state == SEG_MENU_OFF && segment2.state == SEG_MENU_OFF)
		{
			segment1.state = SEG_OFF;
			segment2.state = SEG_OFF;
			state_menu = 0;
		}
	}
}

void init_menu(void)
{
	button1.fall_callback = button_1_menu;
	button2.fall_callback = button_2_menu;
	button3.fall_callback = button_3_menu;
	button5.fall_callback = button_5_menu;
	button6.fall_callback = button_6_menu;
	button7.fall_callback = button_7_menu;
	button8.fall_callback = button_8_menu;
	button9.fall_callback = button_9_menu;
	button10.fall_callback = button_10_menu;
	read_menu_parametrs();
	if (menu_param.motor_add > 99) menu_param.motor_add = 0;
	if (menu_param.servo_add > 99) menu_param.servo_add = 0;
	if (menu_param.disable_error > 1) menu_param.disable_error = 0;
	
}

void modify_tab(uint16_t *tab, uint8_t level, uint8_t size)
{
	uint8_t range = size / 2;
	int n_lvl;
	if (level >= 50)
	{
		n_lvl = level - 50;
		for (uint8_t i = 0; i < size; i++)
		{
			if (i >=0 && i<range)
			tab[i] = tab[i] + (int)tab[i]*n_lvl/50;
			
			else if (i >=range && i<2*range)
			tab[i] = tab[i] + (int)tab[i]*n_lvl/100;
			
			else if (i > 2*range)
			tab[i] = tab[i] + (int)tab[i]*n_lvl/100;
		}
	}
	else
	{
		n_lvl = 50 - level;
		for (uint8_t i = 0; i < size; i++)
		{
			if (i >=0 && i<range)
			tab[i] = tab[i] - (int)tab[i]*n_lvl/100;
			
			else if (i >=range && i<2*range)
			tab[i] = tab[i] - (int)tab[i]*n_lvl/75;
			
			else if (i > 2*range)
			tab[i] = tab[i] - (int)tab[i]*n_lvl/75;
		}
	}
	
}
void modify_value(uint16_t *val, uint8_t level)
{
	int n_lvl;
	if (level >= 50)
	{
		n_lvl = level - 50;
		*val = *val + (int)(*val)*n_lvl/10;
	}
	else
	{
		n_lvl = 50 - level;
		*val = *val - (int)(*val)*n_lvl/25;
	}
}

#endif