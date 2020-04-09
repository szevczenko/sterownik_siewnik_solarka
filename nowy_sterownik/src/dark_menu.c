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

/*
 * Dla zmiany hasla wejscia do dark_menu edytowac password[]
 */
const uint8_t password[] = {1,0,2,4,3,5};
uint8_t pass[sizeof(password)];
uint8_t pass_len;
uint8_t dark_menu_state;
uint8_t configured_parameter;
menuPStruct_t menuParameters[] = 
{
	[MENU_ERROR_SERVO] = {.max_value = 1, .default_value = 0},
	[MENU_ERROR_MOTOR] = {.max_value = 1, .default_value = 0},
	[MENU_ERROR_SERVO_CALIBRATION] = {.max_value = 99, .default_value = 20},
	[MENU_ERROR_MOTOR_CALIBRATION] = {.max_value = 99, .default_value = 50},
};
uint8_t menuSaveParameters[4];

uint8_t dark_menu_parameters_len(void)
{
	return sizeof(menuSaveParameters);
}

static void enter_to_menu(void);

static void save_parametrs(void)
{
	mem_save_data(MEM_CALIBRATION, menuSaveParameters);
}

static void read_menu_parametrs(void)
{
	mem_read_data(MEM_CALIBRATION, menuSaveParameters);
	debug_msg("menu_read %d %d %d %d\n\r", menuSaveParameters[0], menuSaveParameters[1], menuSaveParameters[2], menuSaveParameters[3]);
}

//powinno byc uruchomiane po wczytaniu z EEPROAM
static void check_menu_parameters(void)
{
	uint8_t check_error = 0;
	for(uint8_t i = 0; i < sizeof(menuSaveParameters); i++)
	{
		if (menuSaveParameters[i] > menuParameters[i].max_value)
		{
			menuSaveParameters[i] = menuParameters[i].default_value;
			check_error++;
		}
	}
	if (check_error > 0)
	{
		save_parametrs();
	}
}

static void set_default_parameters(void)
{
	for(uint8_t i = 0; i < sizeof(menuSaveParameters); i++)
	{
		menuSaveParameters[i] = menuParameters[i].default_value;
	}
}

static void clear_password(void)
{
	pass_len = 0;
	memset(pass, 0, sizeof(pass));
}

static void pass_add_number(uint8_t number)
{
	pass[pass_len++] = number;
	debug_msg("pass %d\n", number);
	if (memcmp(pass, password, pass_len) != 0) {
		debug_msg("clear_password\n");
		clear_password();
		return;
	}
	if (pass_len == sizeof(pass)) {
		debug_msg("enter_to_menu\n");
		enter_to_menu();
	}
}

static void button_1_number(void *pv)
{
	pass_add_number(0);
}

static void button_2_number(void *pv)
{
	pass_add_number(1);
}
static void button_3_number(void *pv)
{
	pass_add_number(2);
}
static void button_5_number(void *pv)
{
	pass_add_number(3);
}
static void button_6_number(void *pv)
{
	pass_add_number(4);
}
static void button_7_number(void *pv)
{
	pass_add_number(5);
}

static void button_debug_msg(void)
{
	debug_msg("conf: %d\n", configured_parameter);
	debug_msg("menu after check %d %d %d %d\n\r", menuSaveParameters[0], menuSaveParameters[1], menuSaveParameters[2], menuSaveParameters[3]);
}

static void button_1_menu(void *pv) 
{
	if (configured_parameter > 0)
		configured_parameter--;
	button_debug_msg();
}
static void button_2_menu(void *pv)
{
	if (configured_parameter < sizeof(menuSaveParameters) - 1)
		configured_parameter++;
	button_debug_msg();
}
static void button_3_menu(void *pv)
{
	save_parametrs();
	segment1.state = SEG_MENU_SAVE; 
	segment2.state = SEG_MENU_SAVE;
	LED_MOTOR_OFF;
	LED_SERVO_OFF;
}

static void button_5_menu(void *pv)
{
	if (menuSaveParameters[configured_parameter] > 0)
		menuSaveParameters[configured_parameter]--;
	button_debug_msg();
}
static void button_6_menu(void *pv)
{
	if (menuSaveParameters[configured_parameter] < menuParameters[configured_parameter].max_value)
		menuSaveParameters[configured_parameter]++;
	button_debug_msg();
}
static void button_7_menu(void *pv)
{	
	set_default_parameters();
}

void menu_process(void)
{
	static timer_t menu_timer;
	if (menu_timer < mktime.ms && dark_menu_state == 1)
	{
		menu_timer = mktime.ms + 50;
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
			dark_menu_state = 0;
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
	clear_password();
}

uint8_t dark_menu_get_value(menuParam_t param)
{
	if (param < MENU_ERROR_LAST_PARAM) {
		return menuSaveParameters[param];
	}
	return 0;
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