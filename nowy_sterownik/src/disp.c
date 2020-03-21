/*
 * disp.c
 *
 * Created: 02.02.2019 19:26:01
 *  Author: Demetriusz
 */ 

#include <config.h>
#include <avr/io.h>
#include "disp.h"

disp_drv* disp_tab[LED_DISP_CNT];


#if LED_DISP_CNT>0
#define LED_DISP1
disp_drv DispD1;
#endif

#if LED_DISP_CNT>1
#define LED_DISP2
disp_drv DispD2;
disp_segment_t segment1;
#endif

#if LED_DISP_CNT>2
#define LED_DISP3
disp_drv DispD3;
#endif

#if LED_DISP_CNT>3
#define LED_DISP4
disp_drv DispD4;
disp_segment_t segment2;
#endif

static char displaying_value = 255;

static void clear_display(void)
{
	LED_OFF_A;
	LED_OFF_B;
	LED_OFF_C;
	LED_OFF_D;
	LED_OFF_E;
	LED_OFF_F;
	LED_OFF_G;
}

void off_all_displays(void)
{
	#ifdef LED_DISP1
	LED_OFF_DISP1;
	DispD1.status = 0;
	#endif
	#ifdef LED_DISP2
	LED_OFF_DISP2;
	DispD2.status = 0;
	#endif
	#ifdef LED_DISP3
	LED_OFF_DISP3;
	DispD3.status = 0;
	#endif
	#ifdef LED_DISP4
	LED_OFF_DISP4;
	DispD4.status = 0;
	#endif
}

void init_display_drv(void)
{
	// DDR Config
	SET_PIN(LED_DDR_A, LED_PIN_A);
	SET_PIN(LED_DDR_B, LED_PIN_B);
	SET_PIN(LED_DDR_C, LED_PIN_C);
	SET_PIN(LED_DDR_D, LED_PIN_D);
	SET_PIN(LED_DDR_E, LED_PIN_E);
	SET_PIN(LED_DDR_F, LED_PIN_F);
	SET_PIN(LED_DDR_G, LED_PIN_G);
	
#ifdef LED_DISP1
	SET_PIN(LED_DDR_DISP1, LED_PIN_DISP1);
	disp_tab[0] = &DispD1;
#endif

#ifdef LED_DISP2
	SET_PIN(LED_DDR_DISP2, LED_PIN_DISP2);
	disp_tab[1] = &DispD2;
	segment1.display[0] = &DispD2;
	segment1.display[1] = &DispD1;
#endif

#ifdef LED_DISP3
	SET_PIN(LED_DDR_DISP3, LED_PIN_DISP3);
	disp_tab[2] = &DispD3;
#endif

#ifdef LED_DISP4
	SET_PIN(LED_DDR_DISP4, LED_PIN_DISP4);
	disp_tab[3] = &DispD4;
	segment2.display[0] = &DispD3;
	segment2.display[1] = &DispD4;
#endif
	off_all_displays();
	clear_display();
}

void display_set_value(disp_drv * DispD)
{
	if (DispD->value != displaying_value)
	{
		//if (DispD == &DispD2) TOGGLE_PIN(LED1_PORT, LED1_PIN);
		clear_display();
		switch(DispD->value)
		{
			case 0:
			LED_ON_A;
			LED_ON_B;
			LED_ON_C;
			LED_ON_D;
			LED_ON_E;
			LED_ON_F;
			break;
			case 1:
			LED_ON_B;
			LED_ON_C;
			break;
			case 2:
			LED_ON_A;
			LED_ON_B;
			LED_ON_G;
			LED_ON_E;
			LED_ON_D;
			break;
			case 3:
			LED_ON_A;
			LED_ON_B;
			LED_ON_G;
			LED_ON_C;
			LED_ON_D;
			break;
			case 4:
			LED_ON_F;
			LED_ON_G;
			LED_ON_B;
			LED_ON_C;
			break;
			case 5:
			LED_ON_A;
			LED_ON_F;
			LED_ON_G;
			LED_ON_C;
			LED_ON_D;
			break;
			case 6:
			LED_ON_A;
			LED_ON_F;
			LED_ON_G;
			LED_ON_C;
			LED_ON_D;
			LED_ON_E;
			break;
			case 7:
			LED_ON_A;
			LED_ON_B;
			LED_ON_C;
			break;
			case 8:
			LED_ON_A;
			LED_ON_B;
			LED_ON_C;
			LED_ON_D;
			LED_ON_E;
			LED_ON_F;
			LED_ON_G;
			break;
			case 9:
			LED_ON_A;
			LED_ON_B;
			LED_ON_C;
			LED_ON_D;
			LED_ON_F;
			LED_ON_G;
			break;
			case 'a':
			LED_ON_A;
			LED_ON_B;
			LED_ON_C;
			LED_ON_E;
			LED_ON_F;
			LED_ON_G;
			break;
			case 'b':
			LED_ON_C;
			LED_ON_D;
			LED_ON_E;
			LED_ON_F;
			LED_ON_G;
			break;
			case 'c':
			LED_ON_A;
			LED_ON_D;
			LED_ON_E;
			LED_ON_F;
			break;
			case 'd':
			LED_ON_B;
			LED_ON_C;
			LED_ON_D;
			LED_ON_E;
			LED_ON_G;
			break;
			case 'e':
			LED_ON_A;
			LED_ON_D;
			LED_ON_E;
			LED_ON_F;
			LED_ON_G;
			break;
			case 'f':
			LED_ON_A;
			LED_ON_E;
			LED_ON_F;
			LED_ON_G;
			break;
			case 'r':
			LED_ON_E;
			LED_ON_G;
			break;
			case 'o':
			LED_ON_C;
			LED_ON_D;
			LED_ON_E;
			LED_ON_G;
			break;
			case 'p':
			LED_ON_A;
			LED_ON_B;
			LED_ON_E;
			LED_ON_F;
			LED_ON_G;
			break;
			case 's':
			LED_ON_A;
			LED_ON_C;
			LED_ON_D;
			LED_ON_F;
			LED_ON_G;
			break;
			case 'l':
			LED_ON_D;
			LED_ON_E;
			LED_ON_F;
			break;
			case DISP_OFF:
			break;		
			case SYMBOL_DISP_OFF:
			LED_ON_G;
			break;
			case 254:
			LED_ON_F;
			break;
			case 253:
			LED_ON_E;
			break;
			case 252:
			LED_ON_D;
			break;
			case 251:
			LED_ON_C;
			break;
			case 250:
			LED_ON_B;
			break;
			case 249:
			LED_ON_A;
			break;
		}
		displaying_value = DispD->value;
	}
	
}
void on_display(disp_drv *DispD)
{
	off_all_displays();
	display_set_value(DispD);
	#ifdef LED_DISP1
	if(DispD == &DispD1)
	{
		LED_ON_DISP1;
		DispD1.status = 1;
	}
	#endif
	#ifdef LED_DISP2
	else if(DispD == &DispD2)
	{
		LED_ON_DISP2;
		DispD2.status = 1;
	}
	#endif
	#ifdef LED_DISP3
	else if(DispD == &DispD3)
	{
		LED_ON_DISP3;
		DispD3.status = 1;
	}
	#endif
	#ifdef LED_DISP4
	else if(DispD == &DispD4)
	{
		LED_ON_DISP4;
		DispD4.status = 1;
	}
	#endif
}
static uint8_t iteration = 0;
static uint8_t light = DISP_LIGHT;
void display_process(uint16_t *counter)
{
	*counter = *counter + 1;
	uint16_t cnt = *counter;
	if (cnt % (COUNTER_MAX/LED_DISP_CNT) == 0)
	{
		if (iteration<LED_DISP_CNT)
			on_display(disp_tab[iteration]);
		else
			off_all_displays();
		iteration++;
		if (iteration==LED_DISP_CNT + light) iteration = 0;
		if (cnt == 10*COUNTER_MAX) *counter = 0; 	
	}
}

void disp_set_number(disp_segment_t *segment ,uint8_t number)
{
	if (number >= 100) return;
	else if (number == 0)
	{
		segment->display[0]->value = 0;
		segment->display[1]->value = 0;
		
	}
	else
	{
		segment->display[0]->value = number/10;
		segment->display[1]->value = number%10;
	}
	
}
void disp_set_symb(disp_drv *DispD, char symbol)
{
	DispD->value = symbol;
}