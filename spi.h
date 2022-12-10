/*
 * spi.h
 *
 * Created: 4/3/2022 8:45:10 AM
 *  Author: DEIE
 */ 


#ifndef SPI_H_
#define SPI_H_

inline void set_SS_Low (void){
	PORTB &= 0xFE;
}

inline void set_SS_High (void){
	PORTB |= 0x01;
}

void SPI_Master_Init(void)
{
	/* Set MOSI and SCK output, all others input */
	DDRB |= ((1<<DDB2)|(1<<DDB1)|(1<<DDB0)); //SPI pins on port B MOSI SCK,SS outputs
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = ((1<<SPE)|(1<<MSTR)|(1<<SPR0));//|(1<<CPOL)|(1<<CPHA));
}
unsigned int SPI_Master_Transmit(char cData){
	SPDR = cData;
	set_SS_Low();
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
	set_SS_High();	
	return SPDR;

}
char SPI_Master_Receive(void){
	return 0;
}



#endif /* SPI_H_ */