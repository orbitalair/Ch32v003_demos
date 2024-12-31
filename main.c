/* CH32v003fun Template app on which you can build your own. */

/* This is a demo using the template and merging a 3rd party general lib_i2c
   with both the Pcf8563 Real Time Clock and the SSD1306 driver from CH32v003fun.
   Strictly speaking the OLED display si not necessary to test the RTC.
   Just comment out all the SSD1306 code, and use printf to get output.

   I2C - I used 4.7kohm resistors on the I2C bus as pullups.  You may have to adjust.

   This file build to this size on my Ubuntu 24 linux system:	
   Memory region         Used Size  Region Size  %age Used
           FLASH:        7996 B        16 KB     48.80%
             RAM:        1468 B         2 KB     71.68%

*/

/* what type of OLED - uncomment just one */
/*#define SSD1306_64X32*/
/*#define SSD1306_128X32*/
#define SSD1306_128X64

#include "ch32v003fun.h"
#include "lib_i2c.h"
#include "pcf8563.h"
#include <stdio.h>
#include "ssd1306_i2c.h"
#include "ssd1306.h"
#include "ch32v003_touch.h"

uint32_t count;

/* I2C Scan Callback example function. Prints the address which responded */
void i2c_scan_callback(const uint8_t addr)
{
	printf("Address: 0x%02X Responded.\n", addr);
}

/* Lets test out some features. */
int main()
{
	SystemInit();

	/* Initialise the I2C Interface on the selected pins, at the specified Hz.
	   Enter a clock speed in Hz (Weirdness happens below 10,000), or use one
	   of the pre-defined clock speeds:
	   I2C_CLK_10KHZ    I2C_CLK_50KHZ    I2C_CLK_100KHZ    I2C_CLK_400KHZ
	   I2C_CLK_500KHZ   I2C_CLK_600KHZ   I2C_CLK_750KHZ    I2C_CLK_1MHZ  */
	if(i2c_init(I2C_CLK_400KHZ) != I2C_OK) printf("Failed to init the I2C Bus\n");

	/* Initialising I2C causes the pins to transition from LOW to HIGH.
	   Wait 100ms to allow the I2C Device to timeout and ignore the transition.
	   Otherwise, an extra 1-bit will be added to the next transmission */
	Delay_Ms(100);

	/* Scan the I2C Bus, prints any devices that respond */
	printf("----Scanning I2C Bus for Devices---\n");
	i2c_scan(i2c_scan_callback);
	printf("----Done Scanning----\n\n");

	pcf8563_init();	
	printf("Clock set...\n");
	ssd1306_init();
	/* configure touch pins, enable GPIOC and ADC */
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1;
	InitTouchADC();
	uint32_t but[4] = { 0 };

	pcf8563_set_squarewave(SQW_DISABLE); /*disable the CLK_OUT test pin*/
	/* test set alarm */
	pcf8563_set_alarm(1,2,3,4);
	pcf8563_get_alarm();
	printf("Alarm enabled: 0x%02x, active: 0x%02x\n",pcf8563_alarm_enabled(), pcf8563_alarm_active());

	while(1)
	{
		ssd1306_drawstr_sz(0,0, "[] Date Reminder", 1, fontsize_8x8);
		ssd1306_drawstr_sz(0,20, pcf8563_format_time(RTCC_TIME_HMS), 1, fontsize_16x16);
		ssd1306_drawstr_sz(0,48, pcf8563_format_date(RTCC_DATE_US), 1, fontsize_8x8);
		ssd1306_refresh();			

		int iterations = 3;
		but[0] = ReadTouchPin( GPIOC, 4, 2, iterations );
		/* attach a probe wire to PC4, when you touch it, it should trigger this section of code */
		/* print but[0] to the screen if it isnt working to see your cap touch raw values */
		if (but[0]>5500) { 
			printf("cap button pressed\n"); 
			pcf8563_format_alarm();  /*print alarm bytes*/
			pcf8563_format_status(); /*print the status bytes*/
			pcf8563_clear_alarm(); /*clear alarm, press the wire again to see cleared bytes*/
			pcf8563_set_squarewave(SQW_1024HZ);	/*set the CLK_OUT to 1khz, attach a logic analyzer to verify*/
		}
		Delay_Ms(300); /*play with this to get decent second value prints to the display, can be 'jumpy' */
	}
	return(0);
}

