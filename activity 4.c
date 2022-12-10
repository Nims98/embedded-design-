#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define STANDBY_INIT 0
#define STANDBY 1
#define ACTIVE_INIT 2
#define ACTIVE 3
#define PREPARE_STANDBY 4

#define MOTOR_INIT 0
#define MOTOR1_INIT 1
#define MOTOR1 2
#define MOTOR2_INIT 3
#define MOTOR2 4
#define MOTOR3_INIT 5
#define MOTOR3 6

volatile int buttonPress = 0;
int dState;
int mState;
int timeCount = 0;
int Time;
volatile int ADC_value;

void setDataDirection(void);
void initTimerInterrupt1(void);
void initExternalInterrupt0(void);
void enableTimerInterrupt1(void);
void disableTimerInterrupt1(void);
void motorState(void);
void deviceState(void);
void initAutoTrigger(void);

void setActiveLED(void){ PORTE |=0B00000010;}
void resetActiveLED(void){ PORTE &= 0B11111101;}
void resetStandbyLED(void){ PORTE &= 0B11111110;}
void setStandbyLED(void){ PORTE |=0B00000001;}
void motor1ON(void){ PORTA |= 0B00000001;}
void motor1OFF(void){ PORTA &= 0B11111110;}
void motor2ON(void){ PORTA |= 0B00000010;}
void motor2OFF(void){ PORTA &= 0B11111101;}
void motor3ON(void){ PORTA |= 0B00000100;}
void motor3OFF(void){ PORTA &= 0B11111011;}
void initTimerCount(void){timeCount=0;}


int main(void)
{
	setDataDirection();			// Init the I/O Ports
	initExternalInterrupt0();	// Init the enternal interrupt
	initTimerInterrupt1();		// Init the timer interrupt
	initAutoTrigger();
	sei();						// Enable global interrupt
	/* Initialize */
	dState = STANDBY_INIT;		// Go to STANDBY_INIT state
	while (1)
	{
		deviceState();			// Device state machine
	}
}

void motorState(void)
{
	switch(mState){
		case MOTOR_INIT:
		enableTimerInterrupt1();				// Enable timer 1
		motor1OFF();motor2OFF();motor3OFF();	// Turn all Motors OFF
		initTimerCount();		// Reset the timer and display
		mState = MOTOR1_INIT;					// Move to MOTOR1_INIT state
		break;
		case MOTOR1_INIT:
		motor1ON();motor2OFF();motor3OFF();		// Turn motor 1 ON and motor 2 and motor 3 OFF
		mState = MOTOR1;						// Move to MOTOR1 state
		break;
		case MOTOR1:
		if (timeCount==2)						// If timer = 2
		{
			mState = MOTOR2_INIT;				// Move to MOTOR2_INIT state
		}
		break;
		case MOTOR2_INIT:
		motor1OFF();motor2ON();motor3OFF();		// Turn motor 2 ON and motor 1 and motor 3 OFF
		mState = MOTOR2;						// Move to MOTOR2 state
		break;
		case MOTOR2:
		if (timeCount==6)						// If timer = 6
		{
			mState = MOTOR3_INIT;				// Move to MOTOR3_INIT state
		}
		break;
		case MOTOR3_INIT:
		motor1OFF();motor2OFF();motor3ON();		// Turn motor 1 ON and motor 2 and motor 3 OFF
		mState = MOTOR3;						// Move to MOTOR3 state
		break;
		case MOTOR3:
		if (timeCount==12)						// If timer = 12
		{
			mState = MOTOR1_INIT;				// Move to MOTOR1_INIT state
			initTimerCount();	// Reset the timer and display
		}
		break;
		default:
		;
	}
}

void deviceState(void)
{
	switch(dState){
		case STANDBY_INIT:
		resetActiveLED();			// Turn Active LED OFF
		setStandbyLED();			// Turn Standby LED ON
		disableTimerInterrupt1();	// Disable Timer interrupt 1
		dState = STANDBY;			// Go to STANDBY state
		break;
		case STANDBY:
		if (buttonPress)		// If button is pressed
		{
			buttonPress = 0;		// Reset the button
			dState = ACTIVE_INIT;	// Move to ACTIVE_INIT state
		}
		break;
		case ACTIVE_INIT:
		setActiveLED();			// Turn Active LED ON
		resetStandbyLED();		// Turn Standby LED OFF
		dState = ACTIVE;		// Put device to ACTIVE STATE
		mState = MOTOR_INIT;	// Put motors to MOTOR_INIT state
		PORTB = (ADC_value % 1000)/100;
		PORTL = (((ADC_value % 100)/10)<<4);
		PORTL += (ADC_value % 10);
		break;
		case ACTIVE:
		if (buttonPress)
		{
			buttonPress = 0;			// Reset the button
			dState = PREPARE_STANDBY;	// Move to PREPARE_STANDBY state
			}else{
			motorState();		// Motor state machine
		}
		break;
		case PREPARE_STANDBY:
		/* Turn Motors off and update make the count 0 */
		motor1OFF();motor2OFF();motor3OFF(); // Turn all motors OFF
		initTimerCount();   // Reset the timer and display
		dState = STANDBY_INIT;				// Move to STANDBY_INIT state
		break;
		default:
		;
	}
}

void setDataDirection(void)
{
	DDRD &= 0B11111110;
	DDRB |= 0B00001111;
	DDRA |= 0B00000111;
	DDRL |= 0B11111111;
	DDRE |= 0B00000011;
}


void initTimerInterrupt1(void)
{
	TCNT1 = 0x00;
	TCCR1B |= ((1<<WGM12)|(1<<CS12)|(1<<CS10));
	OCR1A = 46875 - 1;
	enableTimerInterrupt1();
}


void enableTimerInterrupt1(void)
{
	TIMSK1 |= (1<<OCIE1A);	 // Timer 1 compare interrupt 1 enable
}

void disableTimerInterrupt1(void)
{
	TIMSK1 &= ~(1<<OCIE1A);	 // Timer 1 compare interrupt 1 disable
}

ISR(TIMER1_COMPA_vect)
{
	timeCount++;			// Increment the timer
}


void initExternalInterrupt0(void)
{
	EICRA |= 0B00000011; // Trigger at rising edge
	EIMSK |= 0B00000001; // Enable External interrupt 0
}

void initADC(void){
	ADCSRA |= (1<<ADEN);
	ADCSRA |= 0B00000111;
	ADCSRA |= (1<<ADSC);
}

int readADC(uint8_t adc_input_pin){
	ADMUX = (ADMUX & 0xF0) | adc_input_pin;
	ADMUX &= 0x0B00111111;
	ADCSRA |= (1<<ADSC);

	while(ADCSRA & (1<<ADSC));

	return ADC;
}

void initAutoTrigger(void)
{
	ADMUX	= (ADMUX & 0xF0);
	ADCSRB |= 0B00000010;
	ADCSRA  = 0B10111111;
	ADCSRA |= (1<<ADSC);
	sei();
}

ISR(ADC_vect)
{
	ADC_value = ADC;
}

ISR(INT0_vect)
{
	buttonPress = 1;
}