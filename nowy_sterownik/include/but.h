/*
 * but.h
 *
 * Created: 05.02.2019 17:20:49
 *  Author: Demetriusz
 */ 


#ifndef BUT_H_
#define BUT_H_

#define BUTTON_CNT 10 //iloœæ przycisków
#define TIMER_CNT_TIMEOUT 40 

void init_buttons(void);
void process_button(void);

#define BUT1_PIN 2
#define BUT1_PORT PORTB
#define BUT1_DDR DDRB
#define BUT1_RPORT PINB

#define BUT2_PIN 1
#define BUT2_PORT PORTB
#define BUT2_DDR DDRB
#define BUT2_RPORT PINB

#define BUT3_PIN 6
#define BUT3_PORT PORTB
#define BUT3_DDR DDRB
#define BUT3_RPORT PINB

#define BUT4_PIN 3
#define BUT4_PORT PORTD
#define BUT4_DDR DDRD
#define BUT4_RPORT PIND

#define BUT7_PIN 1
#define BUT7_PORT PORTC
#define BUT7_DDR DDRC
#define BUT7_RPORT PINC

#define BUT6_PIN 4
#define BUT6_PORT PORTA
#define BUT6_DDR DDRA
#define BUT6_RPORT PINA

#define BUT5_PIN 3
#define BUT5_PORT PORTB
#define BUT5_DDR DDRB
#define BUT5_RPORT PINB

#define BUT10_PIN 0
#define BUT10_PORT PORTC
#define BUT10_DDR DDRC
#define BUT10_RPORT PINC

#define BUT9_PIN 3
#define BUT9_PORT PORTA
#define BUT9_DDR DDRA
#define BUT9_RPORT PINA

#define BUT8_PIN 4
#define BUT8_PORT PORTB
#define BUT8_DDR DDRB
#define BUT8_RPORT PINB



typedef struct
{
	uint32_t tim_cnt;
	uint8_t state;
	uint8_t value;
	volatile uint8_t *port;
	uint8_t pin;
	void (*rise_callback)(void *button);
	void (*fall_callback)(void *button);
	void (*timer_callback)(void *button);
}but_t;


typedef enum
{
	BUT_UNLOCK,
	BUT_LOCK_TIMER,
	BUT_LOCK,
}   but_state;

extern but_t button1, button2, button3, button4, button5, button6, button7, button8, button9, button10;

uint8_t read_button(but_t *but);

#endif /* BUT_H_ */