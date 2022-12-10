/*
 * Atmega328P-I2C.c
 *
 * Created: 4/3/2022 12:33:32 PM
 * Author : Subodha Gunawardena
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "spi_328p.h"

void setCursorLine(int*);

int main(void)
{
	unsigned char SPI_rxData; // Sata received via SPI
	int index = 0;
	SPI_Slave_Init();
	Lcd_init();
	Lcd_CmdWrite(0x80);
    /* Replace with your application code */
    while (1) 
    {
		SPI_rxData = SPI_Slave_Receive();
		Lcd_DataWrite(SPI_rxData);
		setCursorLine(&index);
		index++;
    }
}

void setCursorLine(int* myIndex)
{
	if (*myIndex % 80){
		if (!(*myIndex % 60)){
			      //Bring cursor to beginning of fourth row
			}else if(!(*myIndex % 40)){
				 //Bring cursor to beginning of third row
			}else if(!(*myIndex % 20)){
				//Bring cursor to beginning of second row
		}
		}else{
		*myIndex = 0;		//Clear the counter to start over again
		Lcd_CmdWrite(0x01); // Clear Display
		Lcd_CmdWrite(0x80); //Bring cursor to beginning of first row
	}
}