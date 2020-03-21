#include <stdint.h>
#include <stdio.h>
#include "accumulator.h"
#include "measure.h"
#include "config.h"

#define ACCUMULATOR_SIZE_TAB 20
#define ACCUMULATOR_ADC_CH 0 //0.028 [V/adc]
#define ACCUMULATOR_HIGH_VOLTAGE 600
#define ACCUMULATOR_LOW_VOLTAGE 395
#define ACCUMULATOR_VERY_LOW_VOLTAGE 350


// 
// ///////////////////////////////////
// //TEST
// typedef uint32_t timer_t;
// int adc_read(int data)
// {
//     return 500;
// }
// int adc0_motor_filter_value = 100;
// 
// uint16_t filtered_value(uint16_t *tab, uint8_t size)
// {
// 	uint16_t ret_val = *tab;
// 	for (uint8_t i = 1; i<size; i++)
// 	{
// 		ret_val = (ret_val + tab[i])/2;
// 	}
// 	return ret_val;
// }
// ///////////////////////////////////

static uint8_t accumulator_state = ACCUM_NORMAL_VOLTAGE;


static float voltage = 0;

float accum_get_voltage(void)
{
    voltage = measure_get_filtered_value(MEAS_ACCUM)*5*5.7/1024 + 0.7;
    return voltage;
}

static uint16_t filtered_accum_adc_val;

void accumulator_process(void)
{
	
	static timer_t accumulator_timer;
	
	if(accumulator_timer < mktime.ms)
	{
		filtered_accum_adc_val = measure_get_filtered_value(MEAS_ACCUM);
		if (filtered_accum_adc_val > ACCUMULATOR_HIGH_VOLTAGE)
		{
			accumulator_state = ACCUM_HIGH_VOLTAGE;
		}
		
		if (filtered_accum_adc_val < ACCUMULATOR_LOW_VOLTAGE)
		{
			accumulator_state =  ACCUM_LOW_VOLTAGE;
		}
		
		if (filtered_accum_adc_val < ACCUMULATOR_VERY_LOW_VOLTAGE)
		{   
			accumulator_state = ACCUM_VERY_LOW_VOLTAGE;
		}
		
		if (filtered_accum_adc_val < ACCUMULATOR_HIGH_VOLTAGE && filtered_accum_adc_val > ACCUMULATOR_LOW_VOLTAGE)
		{
			accumulator_state = ACCUM_NORMAL_VOLTAGE;
		}
		accumulator_timer = mktime.ms + 100;
	}
}

void accum_led_process(void)
{
    static timer_t process_timer;
    static uint8_t state_led;
    if(process_timer < mktime.ms)
	{
        switch (accumulator_state)
        {
        case ACCUM_NORMAL_VOLTAGE:
            LED_ACUM_OFF;
            process_timer +=200;
            break;

        case ACCUM_HIGH_VOLTAGE:
            LED_ACUM_TOGGLE;
		    process_timer += 250;
            break;
        
        case ACCUM_LOW_VOLTAGE:
            if (state_led == 0)
            {
                state_led = 1;
                LED_ACUM_ON;
                process_timer += 500;
            }
            else if (state_led == 1)
            {
                state_led = 0;
                LED_ACUM_OFF;
                process_timer += 1500;
            }
            break;

            case ACCUM_VERY_LOW_VOLTAGE:
                LED_ACUM_ON;
		        process_timer +=1000;
            break;
        default:
            break;
        }
	}
}
