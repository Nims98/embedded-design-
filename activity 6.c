/*
 * Activity 6.c
 *
 * Created: 9/26/2022 12:40:12 PM
 * Author : Nirmala
 */ 

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE ((F_CPU/16/BAUD)-1)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void initUSART(void);
void initHardware(void);
void initADC(void);
void transmitUSART(unsigned char data);
void initExternalInterrupt0();

volatile int rxFlag = 0;
volatile unsigned char rxData;
int currValue =0;
int ADC_value =0;

int main(void)
{
	initExternalInterrupt0();
   initUSART();
   initHardware();
   initADC();
    while (1) 
    {
		//Read the value
		ADMUX = (ADMUX & 0xF0) | 0;	// Set input channel to 0
		ADMUX &= 0B00111111;		// Turn off the internal reference voltage
		ADCSRA |= (1<<ADSC);		//Start the conversion
		
		while(ADCSRA & (1<<ADSC));
		ADC_value = ADC;
		// DIsplay value
		PORTA = (0x00FF & ADC_value);
		PORTC = (0x00C0 & ADC_value >> 2);
		
		if(rxFlag){
			rxFlag=0;
			int a1 = (ADC_value %10);
			int a2 = (ADC_value %100)/10;
			int a3= (ADC_value %1000)/100;
			int a4 = (ADC_value %10000)/1000;
			
			transmitUSART(a4+48);
			transmitUSART(a3+48);
			transmitUSART(a2+48);
			transmitUSART(a1+48);

			transmitUSART(0x0A);
			transmitUSART(0x0D);
			currValue = ADC_value;
    }
	_delay_ms(500);
	}
	return 0;
	}

void initUSART(){
	UBRR0H = (unsigned char)(UBRR_VALUE>>8);		//Set UBRR upper byte
	UBRR0L = (unsigned char)(UBRR_VALUE);		//Set UBRR lower byte
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);		//Enable transmitter/receiver
	UCSR0C = (3<<UCSZ00);											//8 bits data
	
	sei();	// Enable global interrupt
}

void initHardware(){
	DDRA = 0B11111111;
	DDRC = 0B11000000;
	DDRD = 0B11111110;
	DDRE &= 0B00000001;
}

void initADC(void){
	ADCSRA |= (1<<ADEN);
	ADCSRA |= 0B00000111;
	ADCSRA |= (1<<ADSC);
}
void initExternalInterrupt0(void)
{
	EICRA |= 0B00000011; // Trigger at rising edge
	EIMSK |= 0B00000001; // Enable External interrupt 0
}

void transmitUSART(unsigned char data){
	
	while(!(UCSR0A &(1<<UDRE0)));		// wait till the transmit buffer is ready
	UDR0 = data;
}

ISR(INT0_vect)
{
	rxFlag = 1;
}


