#include <stdint.h>
#include "accumulator.h"
#include "disp.h"
#include "display_d.h"
#include "config.h"
#include "system.h"
#include "tim.h"
#include "string.h"

static uint8_t motor_disp_value=0;
static uint8_t servo_disp_value=0;
static uint32_t animation_time;

void dip_set_value(_disp_type type, uint8_t value)
{
    switch(type)
    {
        case DISP_MOTOR:
        motor_disp_value = value;
        break;

        case DISP_SERVO:
        servo_disp_value = value;
        break;

        case DISP_ALL:
        servo_disp_value = value;
        motor_disp_value = value;
        break;
    }
}

int disp_get_state(_disp_type type)
{
	switch(type)
	{
		case DISP_MOTOR:
		return segment1.state;
		break;

		case DISP_SERVO:
		return segment2.state;
		break;

		case DISP_ALL:
		break;
	}
	return -1;
}
disp_state_t disp1_last_state;
disp_state_t disp2_last_state;
void disp_disable_voltage(void)
{
	if (segment1.state == SEG_VOLTAGE)
		segment1.state = disp1_last_state;
	if (segment2.state == SEG_VOLTAGE)
		segment2.state = disp2_last_state;
}

void disp_set_state(_disp_type type, disp_state_t state)
{
    switch(type)
    {
        case DISP_MOTOR:
		if (state == SEG_VOLTAGE)
		{
			disp1_last_state = segment1.state;
		}
		if (segment1.state == SEG_VOLTAGE)
		{
			disp1_last_state = state;
			break;
		}
        segment1.state = state;
        break;

        case DISP_SERVO:
		if (state == SEG_VOLTAGE)
		{
			disp2_last_state = segment2.state;
		}
		if (segment2.state == SEG_VOLTAGE)
		{
			disp2_last_state = state;
			break;
		}
        segment2.state = state;
        break;

        case DISP_ALL:
		if (state == SEG_VOLTAGE)
		{
			disp1_last_state = segment1.state;
			disp2_last_state = segment2.state;
		}
		if (segment1.state == SEG_VOLTAGE)
		{
			disp2_last_state = state;
			disp1_last_state = state;
			break;
		}
        segment1.state = state;
        segment2.state = state;
        break;
    }
	///////////////////////////////
	//debug
	char string[20];
	switch(state)
	{
		case SEG_OFF:
		memcpy(string, "SEG_OFF", 10);
		break;
		case SEG_VALUE:
		memcpy(string, "SEG_VALUE", 10);
		break;
		case SEG_VOLTAGE:
		memcpy(string, "SEG_VOLTAGE", 15);
		break;
		case SEG_ANIMATION:
		memcpy(string, "SEG_ANIMATION", 15);
		break;
		case SEG_ERROR:
		memcpy(string, "SEG_ERROR", 10);
		break;
		case SEG_VIBRO_TIMEOUT:
		memcpy(string, "SEG_VIBRO_TIMEOUT", 20);
		break;
		case SEG_VIBRO_PERIOD:
		memcpy(string, "SEG_VIBRO_PERIOD", 20);
		break;
		case SEG_BLINK:
		memcpy(string, "SEG_BLINK", 20);
		break;
		default:
		memcpy(string, "NONE xD", 10);
		break;

	}
	//debug_msg("DISP: state (%s)\n", string);
}

uint8_t dip_get_value(_disp_type type)
{
    switch(type)
    {
        case DISP_MOTOR:
        return motor_disp_value;
        break;

        case DISP_SERVO:
        return servo_disp_value;
        break;
		case DISP_ALL:
		break;
    }
    return -1;
}

static uint8_t error_reason;

void display_set_error(error_reason_ reason)
{
    segment1.state = SEG_ERROR;
	segment2.state = SEG_ERROR;
    error_reason = reason;
}
static uint16_t voltage;

#if CONFIG_DEVICE_SOLARKA
static timer_t time;
#include "vibro.h"
#endif

static evTime blink_servo_disp;
static uint8_t blink_state;
static uint32_t blink_cnt;
static uint8_t second_disp_blink_val;

void display_set_blink(_disp_type type, uint32_t blink_count, uint8_t blink_val)
{
	blink_cnt = blink_count;
	disp_set_state(type, SEG_BLINK);
	second_disp_blink_val = blink_val;
	//debug_msg("BLINK: blink_count: %d blink_val: %d\n", blink_cnt, second_disp_blink_val);
	if (blink_state == 0) disp_set_number(&segment2, second_disp_blink_val);
}

void disp_off_segments(void)
{
	disp_set_symb(&DispD1, SYMBOL_DISP_OFF);
	disp_set_symb(&DispD2, SYMBOL_DISP_OFF);
	disp_set_symb(&DispD3, SYMBOL_DISP_OFF);
	disp_set_symb(&DispD4, SYMBOL_DISP_OFF);
}


void disp_process(void)
{
	static uint32_t timer_disp_process = 0;
	if (timer_disp_process < mktime.ms)
	{
        
        switch(segment1.state)
        {
            case SEG_VOLTAGE:
			voltage = (int)((float)accum_get_voltage()*(float)100);
            disp_set_number(&segment1, voltage/100);
            break;
            case SEG_VALUE:
            disp_set_number(&segment1, motor_disp_value);
            break;
            case SEG_SAVE:
            if (segment2.state != SEG_SAVE)
            {
                segment1.state = SEG_VALUE;
			    segment2.state = SEG_VALUE;
                break;
            }
        	timer_disp_process += 1000;
			DispD2.value = 's';
			DispD1.value = 'a';
			DispD3.value = 'p';
			segment1.state = SEG_VALUE;
			segment2.state = SEG_VALUE;
			return;
			
            break;
            case SEG_LOAD:
            if (segment2.state != SEG_LOAD)
            {
                segment1.state = SEG_VALUE;
        	    segment2.state = SEG_VALUE;
                break;
            }
			timer_disp_process += 1000;
			DispD2.value = 'l';
			DispD1.value = 'o';
			DispD3.value = 'p';
			segment1.state = SEG_VALUE;
			segment2.state = SEG_VALUE;
            break;
            case SEG_ERROR:
			DispD2.value = 'e';
			DispD1.value = 'r';
			disp_set_number(&segment2, error_reason);
            break;
            case SEG_OFF:
            disp_set_symb(&DispD1, SYMBOL_DISP_OFF);
			disp_set_symb(&DispD2, SYMBOL_DISP_OFF);
            break;
        }

        switch(segment2.state)
        {
            case SEG_VOLTAGE:
            disp_set_number(&segment2, voltage%100);
            break;
            case SEG_VALUE:
            disp_set_number(&segment2, servo_disp_value);
            break;
            case SEG_OFF:
            disp_set_symb(&DispD3, SYMBOL_DISP_OFF);
            disp_set_symb(&DispD4, SYMBOL_DISP_OFF);
            break;
			
			#if CONFIG_DEVICE_SOLARKA
			case SEG_VIBRO_TIMEOUT:
			time = vibro_time_to_timeout_expected();

			if (time == 0)
				display_set_blink(DISP_SERVO, 3, time);
			else
				disp_set_number(&segment2, time/100);
			
			break;

			case SEG_VIBRO_PERIOD:
			time = vibro_time_to_period_end();

			disp_set_number(&segment2, time/100);
			
			break;

			#endif //CONFIG_DEVICE_SOLARKA
			
			case SEG_BLINK:
			if(evTime_process_period(&blink_servo_disp, 500))
			{
				if (blink_state == 0)
				{
					blink_state = 1;
					disp_set_symb(&DispD3, DISP_OFF);
					disp_set_symb(&DispD4, DISP_OFF);
				}
				else
				{
					blink_state = 0;
					if (blink_cnt == 0) 
					{
						disp_set_state(DISP_SERVO, SEG_VALUE);
						break;
					}
					blink_cnt--;
					//debug_msg("BLINK: blink_count: %d blink_val: %d\n", blink_cnt, second_disp_blink_val);
					disp_set_number(&segment2, second_disp_blink_val);
				}
			}
			break;
			
        }	
		timer_disp_process += 50;
	}
}

void on_animation(uint32_t time, _disp_type type)
{
    animation_time = mktime.ms + time;
	disp_set_state(type,SEG_ANIMATION);
}

void animation(void)
{
    static uint32_t timer_anim;
	static uint8_t anim_iteration = 249;
	static uint8_t anim_iteration2 = 254;
	if ((segment1.state == SEG_ANIMATION || segment2.state == SEG_ANIMATION) && timer_anim < mktime.ms)
	{
		timer_anim = mktime.ms + 175;
		if (segment1.state == SEG_ANIMATION)
		{
			disp_set_symb(&DispD1, anim_iteration);
			disp_set_symb(&DispD2, anim_iteration);
		}
		if (segment2.state == SEG_ANIMATION)
		{
			disp_set_symb(&DispD3, anim_iteration2);
			disp_set_symb(&DispD4, anim_iteration2);
		}
		/* Skonczyl sie czas animacji */
		if (animation_time < mktime.ms) 
	    {
			disp_set_state(DISP_ALL, SEG_VALUE);
			LED_ACUM_OFF;
		}
		
		anim_iteration++;
		anim_iteration2--;
		if (anim_iteration == 255) anim_iteration = 249;
		if (anim_iteration2 == 248) anim_iteration2 = 254;
	} 
}