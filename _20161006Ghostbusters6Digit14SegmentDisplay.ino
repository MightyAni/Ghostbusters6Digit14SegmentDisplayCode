//The MAX6955 chip communicates over I2C.  Two (2) pins are required to
//interface.There are multiple selectable I2C addresses.

#include <MAX6955.h>

#ifdef __AVR__
#include <avr/power.h>
#include <avr/wdt.h>	//We're going to set up the watchdog timer
#endif

//We are using the first device address.
//Connecting AD1 and AD0 to GND assigns this address.  
//(See:  MAX6955 Datasheet)
#define ID_MAX6955 B1100000	
						

#define NUMDISPLAYS 6		//We have 6 digits

#define max6955_reg_decodeMode      0x01
#define max6955_reg_globalIntensity 0x02
#define max6955_reg_scanLimit       0x03
#define max6955_reg_configuration   0x04
#define max6955_reg_displayTest     0x07
#define max6955_reg_digitType       0x0C


int writeMAX6955(char command, char data)
{
	Wire.beginTransmission(ID_MAX6955);
	Wire.write(command);
	Wire.write(data);
	Wire.endTransmission();
}

void initMAX6955()
{
	Wire.begin();
	// ascii decoding for all digits;
	writeMAX6955(max6955_reg_decodeMode, 0xFF);
	
	// brightness: 0x00 =  1/16 (min on)  2.5 mA/seg;
	// 0x0F = 15/16 (max on) 37.5 mA/segment
	writeMAX6955(max6955_reg_globalIntensity, 0x0F);
	
	// active displays: 0x07 -> all;
	writeMAX6955(max6955_reg_scanLimit, 0x07);
	
	// set normal operation;
	writeMAX6955(max6955_reg_configuration, 0x01);
	
	// segments/display: 0xFF=14-seg; 0=16 or 7-seg;
	writeMAX6955(max6955_reg_digitType, 0xFF);
	
	// display test off;
	writeMAX6955(max6955_reg_displayTest, 0x00);
}

void setup()
{
	//serial.begin(115200);
//	// This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
//#if defined (__AVR_ATtiny85__)
//	if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
//#endif
	//// set D8 and D9 to GND (for I2C address);
	//pinMode(8, OUTPUT);
	//pinMode(9, OUTPUT);
	//digitalWrite(8, LOW);
	//digitalWrite(9, LOW);

	initMAX6955();
	delay(100);
	clear();
	//  writeDisplay("HI");
	//  writeChar(0, 'A', true);
	//  scrollDisplay("     HELLO WORLD", 300);
	//writeTime(15, 24, 39);

	/*Turn on the watchdog timer:
	"Basic Watchdog Timer" by Nicolas Larsen (11 June 2011)
	--------------------------------------------------------------------------
	128	 |	64	 |	32	 |	16	 |	8	 |	4	 |	2	 |	1
	WDIF	 |	WDIE	 |	WDP3	 |	WDCE	 |	WDE	 |	WDP2	 |	WDP1	 |	WDP0
	--------------------------------------------------------------------------
	0		0		0		0		1		1		1		0	=  14

	This is my configuration.  
	*/
	WDTCSR |= 24;	//Enter watchdog configuration mode
	WDTCSR = 14;	//Enable my configuration (1 second)

}

void loop()
{
	delay(100);
	clear();
	//writeDisplay("HI");
	//writeChar(0, 'A', true);
	//scrollDisplay("288765894140734567810152", random(6000, 8500));
	scrollDisplay("       WHO YOU GONNA CALL     ", 400);
	//writeTime(15, 24, 39);
}

void writeDisplay(char* msg)
{
	for (byte i = 0; i < NUMDISPLAYS; i++)
	{
		if (i < strlen(msg))
			writeMAX6955(0x20 + i, msg[i]);
		else
			writeMAX6955(0x20 + i, ' ');
	}
}

void writeChar(byte pos, char letter, boolean dotLit)
{
	writeMAX6955(0x25 - pos, (dotLit ? 0x80 : 0) | letter);
}

void clear()
{
	for (byte i = 0; i < NUMDISPLAYS; i++)
			writeMAX6955(0x25 - i, ' ');
}

void scrollDisplay(char* msg, int delayMs)
{
	for (int i = 0; i <= strlen(msg) - NUMDISPLAYS; i++)
	{
		wdt_reset();	//We reset the watchdog timer in this loop.
		writeDisplay(msg + i);
		delay(delayMs);
	}
}

//void writeTime(int hours, int minutes, int seconds)
//{
//	char h1 = (hours / 10) ? '0' + hours / 10 : ' ';
//	writeChar(0, h1, false);
//	char h2 = '0' + (hours % 10);
//	writeChar(1, h2, true);
//	char m1 = '0' + (minutes / 10);
//	writeChar(2, m1, false);
//	char m2 = '0' + (minutes % 10);
//	writeChar(3, m2, true);
//	char s1 = '0' + (seconds / 10);
//	writeChar(4, s1, false);
//	char s2 = '0' + (seconds % 10);
//	writeChar(5, s2, false);
//}