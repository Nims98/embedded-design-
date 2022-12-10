/*
 * GccApplication4.c
 *
 * Created: 8/22/2022 11:43:35 PM
 * Author : premi
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void turn_on_01_10(void);
void init_hardware(void);
void grid_led(int index);
void clear(void);
void init_interrupt(void);
void check_y(void);

int y = 0;

int main(void)
{
	init_hardware();
	
	EICRA |= 0x03;
	EIMSK |= 0x01;
	sei();
	
	turn_on_01_10();
	
    /* Replace with your application code */
    while (1) 
    {
		if ( PINA & 0B00000001)
		{
			y+=1;
			grid_led(y);
		}
		else
		{
			clear();
		}
		_delay_ms(200);
    }
}

void turn_on_01_10(void){
	PORTE |= 0B00000001;
	PORTA |= 0B00100000;
}

void clear(void){
	PORTC = 0B00000000;
}

void grid_led(int index){
	switch(index){
		case 1: PORTC = 0B00100100; break;
		case 2: PORTC = 0B00100010; break;
		case 3: PORTC = 0B00100001; break;
		case 4: PORTC = 0B01000001; break;
		case 5: PORTC = 0B10000001; break;
		case 6: PORTC = 0B10000010; break;
		case 7: PORTC = 0B10000100; break;
		case 8: PORTC = 0B01000100; break;
		case 9: PORTC = 0B01000010; break;
		default: y=0; clear();	
	}
}

void init_hardware(void){
	DDRC |= 0B11100111;//setting portC pin 0-2 and 5-7 as output
	DDRE |= 0B00000001;//setting portE pin 1 as output
	DDRA |= 0B00100000;//setting portA pin 5 as output
	DDRA &= 0B11111110;//setting portA pin 0 as input
}


ISR(INT0_vect){
	y+=1;
	grid_led(y);
	_delay_ms(200);
}
