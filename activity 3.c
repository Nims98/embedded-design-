/*
 * Activity 3.c
 *
 * Created: 10/31/2022 12:52:06 PM
 * Author : Nirmala
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define STANDBY_INIT 10
#define STANDBY_RUN 11
#define OPERATING_RUN 21
#define OPERATING_INIT 20

#define MOTOR_INIT 0
#define MOTOR1_INIT 211
#define MOTOR2_INIT 213
#define MOTOR3_INIT 215
#define MOTOR1_RUN 212
#define MOTOR2_RUN 214
#define MOTOR3_RUN 216

#define MOTOR1_LIMIT 4
#define MOTOR2_LIMIT 12
#define MOTOR3_LIMIT 28

volatile int mode = STANDBY_INIT;
volatile int timeIndex = 0;

void setDataDirection(void);
void motorMode(void);

void setActiveLED(void) { PORTE |= 0B00000010; }	  // Turn operating on LED
void resetActiveLED(void) { PORTE &= 0B11111101; }	  // Turn operating off LED
void resetStandbyLED(void) { PORTE &= 0B11111110; }	  // Turn standBy off LED
void setStandbyLED(void) { PORTE |= 0B00000001; }	  // Turn standBy on LED
void motor1ON(void) { PORTA |= 0B00000001; }		  // Turn motor1 on
void motor1OFF(void) { PORTA &= 0B11111110; }		  // Turn motor1 off
void motor2ON(void) { PORTA |= 0B00000010; }		  // Turn motor2 on
void motor2OFF(void) { PORTA &= 0B11111101; }		  // Turn motor2 off
void motor3ON(void) { PORTA |= 0B00000100; }		  // Turn motor3 on
void motor3OFF(void) { PORTA &= 0B11111011; }		  // Turn motor3 off
void enableTimer(void) { TIMSK1 |= (1 << OCIE1A); }	  // Enable timer
void disableTimer(void) { TIMSK1 &= ~(1 << OCIE1A); } // Disable timer
void initExternalInterrupt0(void);					  // Initialize external interrupt 0
void initTimerInt(void);							  // Initialize timer interrupt
void updateDisplay(int myTime) { PORTL = myTime; }	  // Update display

int main(void)
{
	setDataDirection();		  // Init the I/O Ports
	initTimerInt();			  // Init the timer interrupt
	initExternalInterrupt0(); // Init the enternal interrupt
	sei();					  // Enable global interrupt

	while (1)
	{
		motorMode(); // Motor mode
	}
}

void motorMode()
{
	switch (mode)
	{
	case STANDBY_INIT:
		setStandbyLED();
		resetActiveLED();
		motor1OFF();
		motor2OFF();
		motor3OFF();
		updateDisplay(0);
		disableTimer();
		mode = STANDBY_RUN;
		break;
	case STANDBY_RUN:

		break;
	case OPERATING_INIT:
		setStandbyLED();
		setActiveLED();
		mode = OPERATING_RUN;
		break;
	case OPERATING_RUN:
		motor1OFF();
		motor2OFF();
		motor3OFF();
		enableTimer();
		updateDisplay(0);
		mode = MOTOR1_INIT;
		break;
	case MOTOR1_INIT:
		motor1ON();
		motor2OFF();
		motor3OFF();
		mode = MOTOR1_RUN;
		break;
	case MOTOR1_RUN:
		if (timeIndex >= MOTOR1_LIMIT)
		{
			mode = MOTOR2_INIT;
		}
		break;
	case MOTOR2_INIT:
		motor2ON();
		motor1OFF();
		motor3OFF();
		mode = MOTOR2_RUN;
		break;
	case MOTOR2_RUN:
		if (timeIndex >= MOTOR2_LIMIT)
		{
			mode = MOTOR3_INIT;
		}
		break;
	case MOTOR3_INIT:
		motor3ON();
		motor1OFF();
		motor2OFF();
		mode = MOTOR3_RUN;
		break;
	case MOTOR3_RUN:
		if (timeIndex >= MOTOR3_LIMIT)
		{
			mode = MOTOR1_INIT;
		}
		break;
	}
}

void setDataDirection(void)
{
	DDRA |= 0B00000111; // Pin 0-2 of PORTA as output
	DDRL |= 0B00001111; // Pin 0-4 of PORTL as output
	DDRE |= 0B00000011; // Pin 0-1 of PORTE as output
}

void initTimerInt(void)
{
	TCNT1 = 0x00; // Init counter to 0
	// Prescalar to 1024, Mode to CTC
	// TCCR1A no change
	TCCR1B |= ((1 << WGM12) | (1 << CS12) | (1 << CS10)); // Set prescaler 0 to 1
	OCR1A = 46875 - 1;
	disableTimer();
}

ISR(TIMER1_COMPA_vect)
{
	timeIndex++;			  // Increment the timer
	updateDisplay(timeIndex); // Display the timer
}

void initExternalInterrupt0(void)
{
	EICRA |= 0B00000011; // Trigger at rising edge
	EIMSK |= 0B00000001; // Enable External interrupt 0
}

ISR(INT0_vect)
{
	if (mode >= OPERATING_INIT)
	{
		mode = STANDBY_INIT;
	}
	else
	{
		mode = OPERATING_INIT;
	}
}
