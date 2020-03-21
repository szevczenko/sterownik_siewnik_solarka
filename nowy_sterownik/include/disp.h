/*
 * IncFile1.h
 *
 * Created: 02.02.2019 19:25:32
 *  Author: Demetriusz
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

#define LED_PIN_A	6
#define LED_PIN_B	5
#define LED_PIN_C	5
#define LED_PIN_D	7
#define LED_PIN_E	6
#define LED_PIN_F	2
#define LED_PIN_G	7

#define LED_PIN_DISP2 7
#define LED_PIN_DISP1 2
#define LED_PIN_DISP3 4
#define LED_PIN_DISP4 3
				
#define LED_PORT_A	PORTC
#define LED_PORT_B	PORTC
#define LED_PORT_C	PORTA
#define LED_PORT_D	PORTA	
#define LED_PORT_E	PORTA
#define LED_PORT_F	PORTC
#define LED_PORT_G	PORTC

#define  LED_PORT_DISP2 PORTB
#define  LED_PORT_DISP1	PORTD
#define  LED_PORT_DISP3 PORTC
#define  LED_PORT_DISP4 PORTC

#define LED_DDR_A DDRC
#define LED_DDR_B DDRC
#define LED_DDR_C DDRA
#define LED_DDR_D DDRA
#define LED_DDR_E DDRA
#define LED_DDR_F DDRC
#define LED_DDR_G DDRC

#define LED_DDR_DISP2 DDRB
#define LED_DDR_DISP1 DDRD
#define LED_DDR_DISP3 DDRC
#define LED_DDR_DISP4 DDRC

#define LED_ON_A CLEAR_PIN(LED_PORT_A, LED_PIN_A)
#define LED_ON_B CLEAR_PIN(LED_PORT_B, LED_PIN_B)
#define LED_ON_C CLEAR_PIN(LED_PORT_C, LED_PIN_C)
#define LED_ON_D CLEAR_PIN(LED_PORT_D, LED_PIN_D)
#define LED_ON_E CLEAR_PIN(LED_PORT_E, LED_PIN_E)
#define LED_ON_F CLEAR_PIN(LED_PORT_F, LED_PIN_F)
#define LED_ON_G CLEAR_PIN(LED_PORT_G, LED_PIN_G)

#define LED_ON_DISP1 CLEAR_PIN(LED_PORT_DISP1, LED_PIN_DISP1)
#define LED_ON_DISP2 CLEAR_PIN(LED_PORT_DISP2, LED_PIN_DISP2)
#define LED_ON_DISP3 CLEAR_PIN(LED_PORT_DISP3, LED_PIN_DISP3)
#define LED_ON_DISP4 CLEAR_PIN(LED_PORT_DISP4, LED_PIN_DISP4)

#define LED_OFF_A SET_PIN(LED_PORT_A, LED_PIN_A)
#define LED_OFF_B SET_PIN(LED_PORT_B, LED_PIN_B)
#define LED_OFF_C SET_PIN(LED_PORT_C, LED_PIN_C)
#define LED_OFF_D SET_PIN(LED_PORT_D, LED_PIN_D)
#define LED_OFF_E SET_PIN(LED_PORT_E, LED_PIN_E)
#define LED_OFF_F SET_PIN(LED_PORT_F, LED_PIN_F)
#define LED_OFF_G SET_PIN(LED_PORT_G, LED_PIN_G)

#define LED_OFF_DISP1 SET_PIN(LED_PORT_DISP1, LED_PIN_DISP1)
#define LED_OFF_DISP2 SET_PIN(LED_PORT_DISP2, LED_PIN_DISP2)
#define LED_OFF_DISP3 SET_PIN(LED_PORT_DISP3, LED_PIN_DISP3)
#define LED_OFF_DISP4 SET_PIN(LED_PORT_DISP4, LED_PIN_DISP4)

#define SYMBOL_DISP_OFF 248
#define DISP_OFF 247

typedef volatile struct
{
	volatile char value;
	uint8_t status; 
	}disp_drv;
	
typedef volatile struct
{
	disp_drv * display[2];
	uint8_t state;
}disp_segment_t;

void init_display_drv(void);
void display_set_value(disp_drv * DispD);
void display_process(uint16_t *counter);
void on_display(disp_drv *DispD);
void disp_set_number(disp_segment_t *segment ,uint8_t number);
void disp_set_symb(disp_drv *DispD, char symbol);

extern disp_drv DispD1;
extern disp_drv DispD2;
extern disp_drv DispD3;
extern disp_drv DispD4;
extern disp_segment_t segment1;
extern disp_segment_t segment2;


#endif /* INCFILE1_H_ */