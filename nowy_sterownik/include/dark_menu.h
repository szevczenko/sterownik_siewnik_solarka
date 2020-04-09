/*
 * dark_menu.h
 *
 * Created: 13.03.2019 15:26:05
 *  Author: Demetriusz
 */ 


#ifndef DARK_MENU_H_
#define DARK_MENU_H_
#include "config.h"
#if DARK_MENU
typedef struct  
{
	uint8_t motor_add;
	uint8_t servo_add;
	uint8_t disable_error;
}menu_t;

typedef enum
{
	MENU_PASSWORD,
	MENU_PARAMETERS	
}menuState_t;

typedef enum
{
	MENU_ERROR_SERVO,
	MENU_ERROR_MOTOR,
	MENU_ERROR_SERVO_CALIBRATION,
	MENU_ERROR_MOTOR_CALIBRATION,
}menuParam_t;

typedef struct  
{
	uint8_t max_value;
	uint8_t default_value;
}menuPStruct_t;

extern menu_t menu_param;
extern uint8_t menuSaveParameters[4];

void menu_process(void);
void init_menu(void);
void modify_tab(uint16_t *tab, uint8_t level, uint8_t size);
void modify_value(uint16_t *val, uint8_t level);
void read_menu_parametrs(void);

#endif



#endif /* DARK_MENU_H_ */