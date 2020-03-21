#ifndef _DISPLAY_D_
#define _DISPLAY_D_
#include "disp.h"
#include "system.h"

#ifndef ANIMATION_TIME_MS
#define ANIMATION_TIME_MS 1000
#endif

typedef enum{
    DISP_MOTOR,
    DISP_SERVO,
    DISP_ALL
} _disp_type;

typedef enum
{
	SEG_OFF = 0,
	SEG_VALUE,
	SEG_ANIMATION,
	SEG_LOAD,
	SEG_SAVE,
	SEG_ERROR,
	SEG_VOLTAGE,
	SEG_MENU,
	SEG_MENU_SAVE,
	SEG_MENU_DEF,
	SEG_MENU_OFF,
	SEG_VIBRO_TIMEOUT,
	SEG_VIBRO_PERIOD,
	SEG_BLINK,
	
}disp_state_t;


void dip_set_value(_disp_type type, uint8_t value);
void disp_set_state(_disp_type type, disp_state_t state);
int disp_get_state(_disp_type type);
uint8_t dip_get_value(_disp_type type);
void on_animation(uint32_t time, _disp_type type);
void display_set_error(error_reason_ reason);
void display_set_blink(_disp_type type, uint32_t blink_count, uint8_t blink_val);
void disp_disable_voltage(void);

//////////////////////////////////
//PROCESSES
void disp_process(void);
void animation(void);



#endif