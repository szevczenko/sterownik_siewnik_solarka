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
#include "string.h"

#include "tim.h"
#include "dark_menu.h"
#include "eeprom.h"
#include "disp.h"
#include "but.h"
#include "display_d.h"
#include "mem.h"

static uint8_t last_button;
uint8_t state_menu;
menu_t menu_param;
#define MENU_ADDR 12

const uint8_t password[] = {1,0,3,5,4,7};
uint8_t pass[sizeof(password)];
uint8_t pass_len;
uint8_t dark_menu_state;
uint8_t configured_parameter;
menuPStruct_t menuParameters[] = 
{
	[MENU_ERROR_SERVO] = {.max_value = 1, .default_value =0},
	[MENU_ERROR_MOTOR] = {.max_value = 1, .default_value =0},
	[MENU_ERROR_SERVO_CALIBRATION] = {.max_value = 99, .default_value = 50},
	[MENU_ERROR_MOTOR_CALIBRATION] = {.max_value = 99, .default_value = 50},
};
uint8_t menuSaveParameters[4];

static void enter_to_menu(void);

static void save_parametrs(void)
{
	mem_save_data(MEM_CALIBRATION, menuSaveParameters);
}

void read_menu_parametrs(void)
{
	mem_read_data(MEM_CALIBRATION, menuSaveParameters);
	debug_msg("menu_read %d %d %d %d\n\r", menuSaveParameters[0], menuSaveParameters[1], menuSaveParameters[2], menuSaveParameters[3],  )
}

//powinno byc uruchomiane po wczytaniu z EEPROAM
void check_menu_parameters(void)
{
	for(uint8_t i = 0; i < sizeof(menuSaveParameters); i++)
	{
		if (menuSaveParameters[i] > menuParameters[i].max_value)
		{
			menuSaveParameters[i] = menuParameters[i].default_value;
		}
	}
}

static void clear_password(void)
{
	pass_len = 0;
	memset(pass, 0, sizeof(pass));
}

void pass_add_number(uint8_t number)
{
	pass[pass_len++] = number;
	if (memcmp(pass, password, pass_len) != 0) {
		clear_password();
		return;
	}
	if (pass_len == sizeof(pass)) {
		enter_to_menu();
	}
}

void button_1_number(void *pv)
{
	pass_add_number(0);
}

void button_2_number(void *pv)
{
	pass_add_number(1);
}
void button_3_number(void *pv)
{
	pass_add_number(2);
}
void button_5_number(void *pv)
{
	pass_add_number(3);
}
void button_6_number(void *pv)
{
	pass_add_number(4);
}
void button_7_number(void *pv)
{
	pass_add_number(5);
}

void button_1_menu(void *pv)
{
	if (configured_parameter > 0)
		configured_parameter--;
}
void button_2_menu(void *pv)
{
	if (configured_parameter < sizeof(menuSaveParameters))
		configured_parameter++;
}
void button_3_menu(void *pv)
{
	save_parametrs();
	segment1.state = SEG_MENU_SAVE; 
	segment2.state = SEG_MENU_SAVE;
	LED_MOTOR_OFF;
	LED_SERVO_OFF;
}

void button_5_menu(void *pv)
{
	if (configured_parameter > 0)
		menuSaveParameters[configured_parameter]--;
}
void button_6_menu(void *pv)
{
	if (configured_parameter < menuParameters[configured_parameter].max_value)
		menuSaveParameters[configured_parameter]++;
}
void button_7_menu(void *pv)
{	
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
	if (menu_timer < mktime.ms && dark_menu_state == 1)
	{
		if (segment1.state == SEG_MENU)
			disp_set_number(&segment1, configured_parameter);
		if (segment2.state == SEG_MENU)
			disp_set_number(&segment2, menuSaveParameters[configured_parameter]);
			
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
	button1.fall_callback = button_1_number;
	button2.fall_callback = button_2_number;
	button3.fall_callback = button_3_number;
	button5.fall_callback = button_5_number;
	button6.fall_callback = button_6_number;
	button7.fall_callback = button_7_number;
	read_menu_parametrs();
	check_menu_parameters();
	if (menu_param.motor_add > 99) menu_param.motor_add = 0;
	if (menu_param.servo_add > 99) menu_param.servo_add = 0;
	if (menu_param.disable_error > 1) menu_param.disable_error = 0;
}

static void enter_to_menu(void)
{
	dark_menu_state = MENU_PARAMETERS;
	button1.fall_callback = button_1_menu;
	button2.fall_callback = button_2_menu;
	button3.fall_callback = button_3_menu;
	button5.fall_callback = button_5_menu;
	button6.fall_callback = button_6_menu;
	button7.fall_callback = button_7_menu;
	button8.fall_callback = button_8_menu;
	button9.fall_callback = button_9_menu;
	button10.fall_callback = button_10_menu;
	disp_set_state(DISP_ALL, SEG_MENU);
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