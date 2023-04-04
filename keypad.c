#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>

#define LCD_DPRT PORTD
#define LCD_DDR DDRD
#define LCD_DPIN PIND
#define LCD_CPRT PORTB
#define LCD_CDDR DDRB
#define LCD_CPIN PINB

char getKey();

volatile char keyMaps[4][4] = {
	{1,2,3,0},
	{4,5,6,0},
	{7,8,9,0},
	{0,0,0,0},
};

int main(void)
{
	DDRD = 0xFF;
    while(1)
    {
		char x = getKey();
		// PORTD = (int)x;
    }
}

// pin [ROW:COL]
char getKey(){
	// initial row pins
	// to input_pullup
	DDRB |= 0x0F;
	PORTB |= 0xF0;
	char result = 0; // 0 is no pressed
	for (int c=0;c<4;c++) {
		DDRB |=(1<<c);    // cols as output
		PORTB &= ~(1<<c); // write LOW
		for (int r=0; r<4;r++) {
			if((PINB&(1<<(r+4)))==0){
				delay10ms(); // bounce
				while(PINB&(1<<(r+4))==0);
				result = keyMaps[r][c];
			}
		}
		DDRB &= ~(1<<c); // cols as input
		PORTB |= (1<<c); // internal pull-up
	}
	return result;
}

void delay10ms(){
	// 158 = 10ms @32MHz
	TCNT0 = -158;
	TCCR0 = 1<<CS10|1<<CS12;
	while((TIFR&(1<<TOV0))==0);
	TIFR = 1;
	TCCR0 = 0;
	return;
}
