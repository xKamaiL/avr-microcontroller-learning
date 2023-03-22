#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

void initTimer()
{
	TCNT1 = 0;
	TCCR1A |= 0;
	// clk/1024 and CTC mode
	TCCR1B |= 1<<WGM12| 1<<CS12 | 1<<CS10;
	OCR1A = 312;
}

int8_t S[16] = {0};
int8_t T[16] = {0};
int8_t U[16] = {0};
		
int main(void)
{	
	mockEEPROM(); // for testing
	eeprom_read_block(S,(void*)0x010,16);
	eeprom_read_block(T,(void*)0x030,16);
	DDRC = 0xFF;
	PORTC =0;
	DDRD = 0xFF;
	PORTD =0x00;
	initTimer();
	
	TIMSK = 1 << OCIE1A; // timer0 interrupt
	ADCSRA= (1<<ADEN); // enable adc
	ADMUX= 0b01000000; // ref at Vcc=5V
	sei();
	while (1)
	{
		
	}
}

ISR(TIMER1_COMPA_vect){
	// 14->15,13->14 ... 0->1
	for (int i=15;i>0;i--) {
		U[i]=U[i-1];
	}
	
	ADCSRA |= (1<<ADSC);
	while((ADCSRA &(1<<ADIF)) ==0);
	unsigned int raw = ADC; // 16bit
	U[0] = raw>>2; // 10bit to 8bit
	int *B = (PINC&(1<<7)) ? &T : &S; //
	int result = 0; // 20bit
	for (int i=0;i<16;i++){
		result += U[i] * (*(B+i));
	}
	
	// result -> [...4:PORTA:PORTD]
	PORTD = result & 0xFF;
	PORTA = (result >> 8) & 0xFF;
	PORTC = (result >> 16) & 0xF;
}

void mockEEPROM(){
	int x[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	eeprom_write_block(x,(void*)0x010,16);
	eeprom_write_block(x,(void*)0x030,16);
}
