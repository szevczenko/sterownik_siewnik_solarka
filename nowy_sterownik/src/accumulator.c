#include <stdint.h>
#include <stdio.h>
#include "accumulator.h"
#include "measure.h"
#include "config.h"

#define ACCUMULATOR_HIGH_VOLTAGE 16.6 //600
#define ACCUMULATOR_LOW_VOLTAGE 11 //395
#define ACCUMULATOR_VERY_LOW_VOLTAGE 9.7 //350

#define ACCUMULATOR_HIGH_VOLTAGE_24V 31.5 //600
#define ACCUMULATOR_LOW_VOLTAGE_24V 24 //395
#define ACCUMULATOR_VERY_LOW_VOLTAGE_24V 22 //350


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
	#if CONFIG_DEVICE_SOLARKA
    voltage = measure_get_filtered_value(MEAS_ACCUM)*10*5.3/1024 + 0.7;
	#else
	voltage = measure_get_filtered_value(MEAS_ACCUM)*10*5.3/1024;
	#endif
    return voltage;
}

static float accum_voltage;

void accumulator_process(void)
{
	
	static timer_t accumulator_timer;
	
	if(accumulator_timer < mktime.ms)
	{
		accum_voltage = accum_get_voltage();
		if (power_supply_type == POWER_SUPPLY_24V) {
			/* Dla akumulatora 24V */
			if (accum_voltage > ACCUMULATOR_HIGH_VOLTAGE_24V)
			{
				accumulator_state = ACCUM_HIGH_VOLTAGE;
			}
			
			if (accum_voltage < ACCUMULATOR_LOW_VOLTAGE_24V)
			{
				accumulator_state =  ACCUM_LOW_VOLTAGE;
			}
			
			if (accum_voltage < ACCUMULATOR_VERY_LOW_VOLTAGE_24V)
			{
				accumulator_state = ACCUM_VERY_LOW_VOLTAGE;
			}
			
			if (accum_voltage < ACCUMULATOR_HIGH_VOLTAGE_24V && accum_voltage > ACCUMULATOR_LOW_VOLTAGE_24V)
			{
				accumulator_state = ACCUM_NORMAL_VOLTAGE;
			}
		}
		else {
			/* Dla akumulatora 12V */
			if (accum_voltage > ACCUMULATOR_HIGH_VOLTAGE)
			{
				accumulator_state = ACCUM_HIGH_VOLTAGE;
			}
			
			if (accum_voltage < ACCUMULATOR_LOW_VOLTAGE)
			{
				accumulator_state =  ACCUM_LOW_VOLTAGE;
			}
			
			if (accum_voltage < ACCUMULATOR_VERY_LOW_VOLTAGE)
			{
				accumulator_state = ACCUM_VERY_LOW_VOLTAGE;
			}
			
			if (accum_voltage < ACCUMULATOR_HIGH_VOLTAGE && accum_voltage > ACCUMULATOR_LOW_VOLTAGE)
			{
				accumulator_state = ACCUM_NORMAL_VOLTAGE;
			}
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
