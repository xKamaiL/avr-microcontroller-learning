#include <avr/io.h>
#include <avr/interrupt.h>

void initTimer()
{
	// initialize timer
	TCNT0 = 0;
	// CTC mode
	TCCR0A |= 1 << WGM01;
	// no pre scale
	TCCR0B |= 1 << CS00;
	// 0 -> 10 count
	OCR0A = 10;
}

int main(void)
{
	// output of signal
	DDRB = 0xFF;
	PORTB =0x00;
	initTimer();
	// timer0 interrupt
	TIMSK0 = 1 << OCIE0A;
	ADCSRA= (1<<ADEN);
	// refVcc
	ADMUX= 0b01000000;
	sei();
	while (1)
	{
		
	}
}

ISR(TIMER0_COMPA_vect){
	ADCSRA |= (1<<ADSC);
	while((ADCSRA &(1<<ADIF)) ==0);
	uint8_t l = ADCL;
	uint8_t h = ADCH;
	PORTB = h<<6|l>>2;
}
