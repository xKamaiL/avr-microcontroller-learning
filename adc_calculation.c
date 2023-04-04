#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

int8_t S[16] = {0};
int8_t T[16] = {0};
int8_t U[16] = {0};

int main(void)
{
	mockEEPROM(); // for testing
	eeprom_read_block(S, (void *)0x010, 16);
	eeprom_read_block(T, (void *)0x030, 16);
	DDRC = 0xFF;
	PORTC = 0;
	DDRD = 0xFF;
	PORTD = 0x00;
	// timer1 setup
	TCNT1 = 0;
	TCCR1A |= 0;
	// clk/1024 and CTC mode @32MHz
	TCCR1B |= 1 << WGM12 | 1 << CS12 | 1 << CS10;
	OCR1A = 321; // calculation 10ms

	TIMSK = 1 << OCIE1A;  // timer1 interrupt
	ADCSRA = (1 << ADEN); // enable adc
	ADMUX = 0b01000000;	  // ref at Vcc=5V
	sei();
	while (1)
	{
	}
}

ISR(TIMER1_COMPA_vect)
{

	// 14->15,13->14 ... 0->1
	for (int i = 15; i > 0; i--)
	{
		U[i] = U[i - 1];
	}
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA & (1 << ADIF)) == 0){}
	unsigned int raw = ADC;	// 16bit
	U[0] = raw >> 2;	// 10bit to 8bit
	int8_t *B = PINC & (1 << 7) ? &T : &S; // mux T or S
	long result = 0;	// but 20bit
	for (int i = 0; i < 16; i++)
	{
		result += U[i] * (B[i]);
	}
	// result -> [...4:PORTA:PORTD]
	PORTD = result & 0xFF;
	PORTA = (result >> 8) & 0xFF;
	PORTC = (result >> 16) & 0xF;
}

void mockEEPROM()
{
	int8_t x[16] = {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127};
	eeprom_write_block(x, (void *)0x010, 16);
	eeprom_write_block(x, (void *)0x030, 16);
}
