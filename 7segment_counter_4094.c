#include <msp430.h>

/*
 *
 * Using a common cathode 7 segment display with a hc4094 shift register
 *
 *
 * pin0 = strobe	BIT0
 * pin1 = data	BIT1
 * pin2 = clock	BIT2
 * pin3 = oe	BIT3
 */

//Define the ports and pins
#define HC4094_PORT P1OUT

#define HC4094_CLOCK_PIN BIT2
#define HC4094_STROBE_PIN BIT0
#define HC4094_DATA_PIN BIT1


//Tested each bit to figure out which segment it controls
#define MIDDLE 0x7f
#define TOPLEFT 0xbf
#define TOP 0xdf
#define TOPRIGHT 0xef
#define BOTTOMLEFT 0xf7
#define BOTTOM 0xfb
#define BOTTOMRIGHT 0xfd
#define DOT 0xfe

//Combining the right bits to form numbers
#define ONE TOPRIGHT & BOTTOMRIGHT
#define TWO TOP & TOPRIGHT & MIDDLE & BOTTOMLEFT & BOTTOM
#define THREE TOP & TOPRIGHT & MIDDLE & BOTTOMRIGHT & BOTTOM
#define FOUR TOPLEFT & TOPRIGHT & MIDDLE & BOTTOMRIGHT
#define FIVE MIDDLE & TOPLEFT & TOP & BOTTOM & BOTTOMRIGHT
#define SIX MIDDLE & TOPLEFT & BOTTOM & BOTTOMRIGHT & BOTTOMLEFT
#define SEVEN TOP & TOPRIGHT & BOTTOMRIGHT
#define EIGHT MIDDLE & TOPLEFT & TOPRIGHT & TOP & BOTTOM & BOTTOMRIGHT & BOTTOMLEFT
#define NINE MIDDLE & TOPLEFT & TOPRIGHT & TOP & BOTTOMRIGHT
#define ZERO TOPLEFT & TOPRIGHT & TOP & BOTTOM & BOTTOMRIGHT & BOTTOMLEFT

//Function prototypes
void HC4094Pulse(void);
void HC4094Latch(void);
void HC4094Write(int);
void HC4094Show(int);
void delayms(int);

int main(void) {

	WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
	P1DIR |= BIT0 + BIT1 + BIT2 + BIT3; // set p.0, p.1, p.2, p.3 as output
	P1OUT = BIT3; // set OE on
	
	for(;;)
	{
		//patterns
		int i;
		for(i=0;i<5;i++)
		{
			HC4094Write(TOP);
			delayms(100);
			HC4094Write(TOPRIGHT);
			delayms(100);
			HC4094Write(MIDDLE);
			delayms(100);
			HC4094Write(BOTTOMLEFT);
			delayms(100);
			HC4094Write(BOTTOM);
			delayms(100);
			HC4094Write(BOTTOMRIGHT);
			delayms(100);
			HC4094Write(MIDDLE);
			delayms(100);
			HC4094Write(TOPLEFT);
			delayms(100);
		}

		for(i=0;i<5;i++)
		{
			HC4094Write(TOP);
			delayms(100);
			HC4094Write(TOPRIGHT);
			delayms(100);
			HC4094Write(BOTTOMRIGHT);
			delayms(100);
			HC4094Write(BOTTOM);
			delayms(100);
			HC4094Write(BOTTOMLEFT);
			delayms(100);
			HC4094Write(TOPLEFT);
			delayms(100);
		}

		for(i=0;i<5;i++)
		{
			HC4094Write(TOP);
			delayms(100);
			HC4094Write(TOPLEFT);
			delayms(100);
			HC4094Write(MIDDLE);
			delayms(100);
			HC4094Write(BOTTOMRIGHT);
			delayms(100);
			HC4094Write(BOTTOM);
			delayms(100);
			HC4094Write(BOTTOMLEFT);
			delayms(100);
			HC4094Write(MIDDLE);
			delayms(100);
			HC4094Write(TOPRIGHT);
			delayms(100);
		}

		for(i=0;i<8;i++)
		{
			HC4094Write(TOP);
			delayms(100);
			HC4094Write(MIDDLE);
			delayms(100);
			HC4094Write(BOTTOM);
			delayms(100);
			HC4094Write(MIDDLE);
			delayms(100);
		}

		for(i=0;i<10;i++)
		{
			HC4094Write(TOPRIGHT & BOTTOMRIGHT);
			delayms(100);
			HC4094Write(TOPLEFT & BOTTOMLEFT);
			delayms(100);

		}

		//show numbers
		for(i=0;i<10;i++)
		{
			HC4094Show(i);
			delayms(250);
		}

	}

	return 0;
}

void delayms(int ms)
{
	int i;
	for(i=0;i<ms;i++)
	{
		__delay_cycles(1000);
	}
}

//Functions that were used to test the bits, and results
//The hex value in the function corresponts to the byte value on the right
//Since my dislay is common cathode, the bit that is OFF is the one that is lit.

//HC4094Write(0x7f); // 0111 1111 = middle
//HC4094Write(0xbf); // 1011 1111 = top left
//HC4094Write(0xdf); // 1101 1111 = top
//HC4094Write(0xef); // 1110 1111 = top right
//HC4094Write(0xf7); // 1111 0111 = bottom left
//HC4094Write(0xfb); // 1111 1011 = bottom
//HC4094Write(0xfd); // 1111 1101 = bottom right
//HC4094Write(0xfe); // 1111 1110 = dot

//Showing numbers. Takes an int and uses the definitions from above
void HC4094Show(int number)
{
	switch(number)
	{
	case 1:
		HC4094Write(ONE);
		break;
	case 2:
		HC4094Write(TWO);
		break;
	case 3:
		HC4094Write(THREE);
		break;
	case 4:
		HC4094Write(FOUR);
		break;
	case 5:
		HC4094Write(FIVE);
		break;
	case 6:
		HC4094Write(SIX);
		break;
	case 7:
		HC4094Write(SEVEN);
		break;
	case 8:
		HC4094Write(EIGHT);
		break;
	case 9:
		HC4094Write(NINE);
		break;
	case 0:
		HC4094Write(ZERO);
		break;
	default:
		HC4094Write(0xff);
		break;
	}
}

void HC4094Pulse(void)
{
	//Pulse the Shift Clock
	HC4094_PORT |= HC4094_CLOCK_PIN;	//HIGH
	HC4094_PORT &= ~HC4094_CLOCK_PIN;	//LOW
}

//Sends a clock pulse on ST_CP line
void HC4094Latch(void)
{
	//Pulse the Store Clock
	HC4094_PORT |= HC4094_STROBE_PIN;//HIGH
	HC4094_PORT &= ~HC4094_STROBE_PIN;//LOW
}

void HC4094Write(int data)
{
	//Send each 8 bits serially
	int i;
	for(i=0;i<8;i++)
	{
		//Output the data on DS line according to the
		//Value of MSB
		if(data & 0x01)
		{
			//MSB is 1 so output high
			HC4094_PORT |= HC4094_DATA_PIN;
		}
		else
		{
			//MSB is 0 so output low
			HC4094_PORT &= ~HC4094_DATA_PIN;
		}
		HC4094Pulse();  //Pulse the Clock line
		data=data>>1;  //Now bring next bit at MSB position
	}
	//Now all 8 bits have been transferred to shift register
	//Move them to output latch at one
	HC4094Latch();
}
