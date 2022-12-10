/*
 * spi_328p.h
 *
 * Created: 4/3/2022 12:42:04 PM
 *  Author: DEIE
 */ 


#ifndef SPI_328P_H_
#define SPI_328P_H_

void SPI_Slave_Init(void)
{
	/* Set MISO as output, rest as input */
	DDRB = (1<<DDB4);
	/* Enable SPI */
	SPCR = (1<<SPE);
}

char SPI_Slave_Receive(void)
{
	//SPDR = 'A';
	while(!(SPSR & (1<<SPIF)))
	;
	return SPDR;
}



#endif /* SPI_328P_H_ */