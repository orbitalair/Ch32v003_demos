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

#include "ch32fun.h"
#include "lib_i2c.h"
#include <stdio.h>
#include "eeprom.h"

uint32_t count;

/* I2C Scan Callback example function. Prints the address which responded */
void i2c_scan_callback(const uint8_t addr)
{
	printf("Address: 0x%02X Responded.\n", addr);
}

void test_buf_print(uint8_t *buf, uint8_t bufsize){
    int k=0;
    for (k=0;k<bufsize;k++){
        printf("%c",buf[k]);
    }
    printf("\n");
}

void test(void){
    uint8_t buf[1];
    uint8_t buf2[1];
    buf[0]='a';
    buf2[0]=0;
    char buf3[64];
    strncpy(buf3,"this is a long string of values. deadbeef",64);
    uint8_t ret;

    /* char write */
    eep_write(0x0, buf, 1);
    printf("write 0x%x\n", buf[0]);
    /* char read */
    eep_read(0x0, buf2, 1);
    printf("read, should be 'a', %c\n", buf2[0]);
    /* page write */
    printf("page write\n");
    eep_write(0x0, buf3, 64);
    test_buf_print(buf3,64);
    printf("\n");
    /* page read */
    printf("page read\n");
    eep_read(0x0, buf3, 64);
    test_buf_print(buf3,64);
    printf("\n");

    /* not on a boundary but data fits , write data */
    printf("page write\n");
    ret=eep_write(0x10, buf3, 32);
    test_buf_print(buf3,64);
    printf("ret, should be 0, %d\n",ret);
    /* not on a boundary and data wont fit, return -2 */
    printf("page write\n");
    ret=eep_write(0x32, buf3, 64);
    printf("ret, should be 253, %d\n",ret);
    test_buf_print(buf3,64);
    printf("\n");
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

	test();


	return(0);
}

