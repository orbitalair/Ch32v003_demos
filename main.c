/* Template app on which you can build your own. */

// what type of OLED - uncomment just one
//#define SSD1306_64X32
//#define SSD1306_128X32
#define SSD1306_128X64


#include "ch32v003fun.h"
#include "lib_i2c.h"
#include "pcf8563.h"
#include <stdio.h>
#include "ssd1306_i2c.h"
#include "ssd1306.h"
#include "ch32v003_touch.h"

uint32_t count;

// I2C Scan Callback example function. Prints the address which responded
void i2c_scan_callback(const uint8_t addr)
{
	printf("Address: 0x%02X Responded.\n", addr);
}

int main()
{
	SystemInit();


		// Initialise the I2C Interface on the selected pins, at the specified Hz.
	// Enter a clock speed in Hz (Weirdness happens below 10,000), or use one
	// of the pre-defined clock speeds:
	// I2C_CLK_10KHZ    I2C_CLK_50KHZ    I2C_CLK_100KHZ    I2C_CLK_400KHZ
	// I2C_CLK_500KHZ   I2C_CLK_600KHZ   I2C_CLK_750KHZ    I2C_CLK_1MHZ
	if(i2c_init(I2C_CLK_400KHZ) != I2C_OK) printf("Failed to init the I2C Bus\n");
	// the ssd1306 oled i2c runs at 100khz

	// Initialising I2C causes the pins to transition from LOW to HIGH.
	// Wait 100ms to allow the I2C Device to timeout and ignore the transition.
	// Otherwise, an extra 1-bit will be added to the next transmission
	Delay_Ms(100);

	// Scan the I2C Bus, prints any devices that respond
	printf("----Scanning I2C Bus for Devices---\n");
	i2c_scan(i2c_scan_callback);
	printf("----Done Scanning----\n\n");

	/*** Example ***/
	// This example is specifically for the DS3231 I2C RTC Module.
	// Use this as an example for generic devices, changing Address, speed etc
	//i2c_err_t i2c_stat;

	pcf8563_init();	
	printf("clock set...\n");
	//show_oled();
	//if(!ssd1306_i2c_init())
	//{
	ssd1306_init();
	//}
	// Enable GPIOD, C and ADC
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1;

	InitTouchADC();
	uint32_t but[4] = { 0 };

	while(1)
	{
		//printf("I2C Test");
		//pcf8563_get_time();
		//pcf8563_print_time();
		//pcf8563_get_date();
		//pcf8563_print_date();
		ssd1306_drawstr_sz(0,0, "[] Date Reminder", 1, fontsize_8x8);
		ssd1306_drawstr_sz(0,20, pcf8563_format_time(RTCC_TIME_HMS), 1, fontsize_16x16);
		ssd1306_drawstr_sz(0,48, pcf8563_format_date(RTCC_DATE_US), 1, fontsize_8x8);
		ssd1306_refresh();			

		int iterations = 3;
		but[0] = ReadTouchPin( GPIOC, 4, 2, iterations );
		printf("but[0]=%ld\n",but[0]);
		if (but[0]>5500) { printf("cap button pressed"); }

		Delay_Ms(300);
	}
	return(0);
}

