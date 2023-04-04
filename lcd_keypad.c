#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>

#define LCD_DPRT PORTC
#define LCD_DDDR DDRC
#define LCD_DPIN PINC
#define LCD_CPRT PORTA
#define LCD_CDDR DDRA
#define LCD_CPIN PINA
#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2

char getKey();
void delay100us();


volatile char keyMaps[4][4] = {
	{'1','2','3',0},
	{'4','5','6',0},
	{'7','8','9',0},
	{0,0,0,0},
};

void lcdPrint(const char* str){
    int i;
    for(i=0;str[i]!='\0';i++)
		lcdData(str[i]);
}

int main(void)
{
	DDRD =0xFF;
	lcdInit();
	lcdGoToXY(1,1);
    while(1)
    {
		char c = getKey();
		if(c != 0){
			lcdData(c);
		}
    }
}

void lcdCMD(unsigned char cmd){
	LCD_DPRT = cmd;
	LCD_CPRT &= ~(1<<LCD_RS);
	LCD_CPRT &= ~(1<<LCD_RW);
	LCD_CPRT |= (1<<LCD_EN);
	delay100us();
	LCD_CPRT &= ~(1<<LCD_EN);
	delay100us();
}
void lcdData(unsigned char data){
	LCD_DPRT = data;
	LCD_CPRT |= 1<<LCD_RS;
	LCD_CPRT &=~(1<<LCD_RW);
	LCD_CPRT |= 1<<LCD_EN;
	delay100us();
	LCD_CPRT &=~(1<<LCD_EN);
	delay100us();
}
void lcdInit(){
	LCD_CDDR = 0xFF;
	LCD_DDDR = 0xFF;
	LCD_CPRT &=~(1<<LCD_EN);
	delay10ms();
	lcdCMD(0x38);
	lcdCMD(0x0E);
	lcdCMD(0x01);
	delay10ms();
	lcdCMD(0x06);
}
void lcdGoToXY(unsigned char x, unsigned char y){
	unsigned char firstCharAdr[] ={0x80,0xC0,0x94,0xD4};
	lcdCMD(firstCharAdr[y-1]+ x-1);
	delay100us();	
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
				delay100us();
				while((PINB&(1<<(r+4)))==0);
				result = keyMaps[r][c];
			}
		}
		DDRB &= ~(1<<c); // cols as input
		PORTB |= (1<<c); // internal pull-up
	}
	return result;
}
void delay500ms(){
	for(int i=0;i<=25;i++){
		delay10ms();
	}
}
void delay10ms(){
	// 158 = 10ms @32MHz
	TCNT0 = -158;
	TCCR0 = 1<<CS02|1<<CS00;
	while((TIFR&(1<<TOV0))==0);
	TIFR = 0x1;
	TCCR0 = 0;
	return;
}
void delay100us(){
	// @32MHz/64
	// 50 = 100us
	TCNT0 = -50;
	TCCR0 =1<<CS01|1<<CS00;
	while((TIFR&(1<<TOV0))==0);
	TIFR = 1;
	TCCR0 = 0;
	return;
}
