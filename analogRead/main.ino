#define __AVR_ATmega32U4__
#include <avr/interrupt.h>
//

void setup()
{
    DDRB = 0xFF;
    DDRD = 0xFF;
    PORTB = 0x00;
    initTimer();
    sei();
}

void loop()
{
    // read ADC from pin 0
    readData(MUX0);
}

void initTimer()
{
    TCCR0A = 1 << WGM01;
    TCCR0B = 1 << CS00;
}

// pin 0 to 7
void readData(uint8_t p, bool autoTrigger)
{

    ADMUX = 1 << REFS0 | p;
    // enable and start conversion
    // with interrupt
    ADCSRA = 1 << ADSC | 1 << ADIE | 1 << ADEN;
    if (autoTrigger)
    {
        ADCSRB |= 1 << ADATE;
    }
    //
    while (ADCSRA & (1 << ADIF))
    {
    }
}

ISR(ADC_vect)
{
    // uint8_t low = ADCL;
    PORTD = ADCL;
    //
    PORTB = ADCH;
}

//