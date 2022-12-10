/*
 * GccApplication6.c
 *
 * Created: 9/12/2022 1:42:19 PM
 * Author : Nirmala
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LcdDataBus PORTA
#define LcdControlBus PORTA

#define LcdDataBusDirnReg DDRA

#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2

void Lcd_CmdWrite(char);
void Lcd_DataWrite(char);
void Lcd_init(void);
void initPorts();
void Turn_Power_Indicator_LED_ON();
void counter(void);
void initInterrupt();

int count = 0;
int state =0;
int digits;
int digits10;
int digits100;

int main(void)
{
	DDRD &= 0B11111110;
initInterrupt();
  initPorts();
  Turn_Power_Indicator_LED_ON();
  Lcd_init();
  //initUSART();
  Lcd_DataWrite('H');
  Lcd_DataWrite('E');
  Lcd_DataWrite('L');
  Lcd_DataWrite('L');
  Lcd_DataWrite('0');
  while (1)
  {
	  	if(state){
		  	counter();
	  	}
  }
  return 0;
}

void initPorts(void){
	LcdDataBusDirnReg = 0xFF;
	DDRB |= 0B10000000;
}

void Turn_Power_Indicator_LED_ON(void){
	PORTB |= 0B10000000;
}

void Lcd_CmdWrite(char cmd)
{
	LcdDataBus = (cmd & 0xF0); //Set upper 4 bits of the cmd
	LcdControlBus &= ~(1<<LCD_RS);//Set RS pin to LOW
	LcdControlBus &= ~(1<<LCD_RW);//Set RW pin to LOW
	LcdControlBus |= (1<<LCD_EN); //Generate High-to-Low pulse
	
	_delay_ms(10);
	
	LcdControlBus &= ~(1<<LCD_EN);
	
	_delay_ms(10);
	
	LcdDataBus = ((cmd<<4) & 0xF0); //Set lower 4 bits of the cmd
	LcdControlBus &= ~(1<<LCD_RS);//Set RS pin to LOW
	LcdControlBus & ~(1<<LCD_RW);//Set RW pin to LOW
	LcdControlBus |= (1<<LCD_EN); //Generate High-to-Low pulse
	
	_delay_ms(10);
	
	LcdControlBus &= ~(1<<LCD_EN);
	
	_delay_ms(10);
}

void Lcd_DataWrite(char dat)
{
	LcdDataBus = (dat & 0xF0); //Set upper 4 bits of the data
	LcdControlBus |= (1<<LCD_RS);//Set RS pin to High
	LcdControlBus &= ~(1<<LCD_RW);//Set RW pin to LOW
	LcdControlBus |= (1<<LCD_EN); //Generate High-to-Low pulse
	
	_delay_ms(10);
	
	LcdControlBus &= ~(1<<LCD_EN);
	_delay_ms(18);
	
	LcdDataBus = ((dat<<4) & 0xF0); //Set lower 4 bits of the data
	LcdControlBus |= (1<<LCD_RS);//Set RS pin to HIGH
	LcdControlBus &= ~(1<<LCD_RW);//Set RW pin to LOW
	LcdControlBus |= (1<<LCD_EN); //Generate High-to-Low pulse
	_delay_ms(10);
	LcdControlBus &= ~(1<<LCD_EN);
	_delay_ms(18);
	}

void Lcd_init(void)
{
	Lcd_CmdWrite(0x02);			//Initialize Lcd in 4-bit mode
	Lcd_CmdWrite(0x28);		 //enable 5x7 mode for chars
	Lcd_CmdWrite(0x0E);			//Display OFF, Cursor ON
	Lcd_CmdWrite(0x01);			//Clear Display
	Lcd_CmdWrite(0x80);			//Move the cursor to beginning of first line

}

// Counter Function
void counter(void){
digits = count%10;
digits10 = (count%100)/10;
digits100 = (count%1000)/100;

	Lcd_CmdWrite(0x80);		
	_delay_ms(100);
	
	Lcd_DataWrite('0'+digits100);
	Lcd_DataWrite('0'+digits10);
	Lcd_DataWrite('0'+digits);
	_delay_ms(1000);
	count++;
	
}
// Enabling interrupts
void initInterrupt(){
	EICRA |= 0x03;
	EIMSK |= 0x01;
	sei();
}

// Read interrupt and change state
ISR(INT0_vect){
Lcd_CmdWrite(0x01);
	switch(state){
case 0:
	state=1;
	break;
case 1:
	state=0;
	break;
	}
}