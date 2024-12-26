/*****
 *  NAME
 *    External Real Time Clock support routines
 *  AUTHOR
 *    Joe Robertson, jmr
 *    orbitalair@bellsouth.net
 *    http://orbitalair.wikispaces.com/Arduino
 *  CREATION DATE
 *    1/2/10,  init - built off of pic rtc code
 *  NOTES
 *  HISTORY
 *    28/02/2012 A. Pasotti
 *            fixed a bug in RTCC_ALARM_AF,
 *            added a few (not really useful) methods
 *    1/2/10  ported to arduino compiler, jmr
 *    2/14/10 added 3 world date formats, jmr
 *  TODO
 *
 ******
 *  Robodoc embedded documentation.
 *  http://www.xs4all.nl/~rfsber/Robo/robodoc.html
 */

#ifndef Rtc_Pcf8563_H
#define Rtc_Pcf8563_H

#include <stdio.h>
#include <stdint.h>
#include "lib_i2c.h"

/* the read and write values for pcf8563 rtcc */
/* these are adjusted for arduino */
//#define RTCC_R 	0xa3  //a3
//#define RTCC_W 	0xa2  //a2
#define RTCC_ADDR    0x51  //top 7 bits or 0xa2>>1
// the i2c lib auto adds the last 0 or 1 bit for read or write
#define RTCC_SEC 		1
#define RTCC_MIN 		2
#define RTCC_HR 		3
#define RTCC_DAY 		4
#define RTCC_WEEKDAY	5
#define RTCC_MONTH 		6
#define RTCC_YEAR 		7
#define RTCC_CENTURY 	8

/* register addresses in the rtc */
#define RTCC_STAT1_ADDR			0x0
#define RTCC_STAT2_ADDR			0x01
#define RTCC_SEC_ADDR  			0x02
#define RTCC_MIN_ADDR 			0x03
#define RTCC_HR_ADDR 			0x04
#define RTCC_DAY_ADDR 			0x05
#define RTCC_WEEKDAY_ADDR		0x06
#define RTCC_MONTH_ADDR 		0x07
#define RTCC_YEAR_ADDR 			0x08
#define RTCC_ALRM_MIN_ADDR 	    0x09
#define RTCC_SQW_ADDR 	        0x0D

/* setting the alarm flag to 0 enables the alarm.
 * set it to 1 to disable the alarm for that value.
 */
#define RTCC_ALARM				0x80
#define RTCC_ALARM_AIE 			0x02
#define RTCC_ALARM_AF 			0x08 // 0x08 : not 0x04!!!!
/* optional val for no alarm setting */
#define RTCC_NO_ALARM			99

#define RTCC_CENTURY_MASK 		0x80

/* date format flags */
#define RTCC_DATE_WORLD			0x01
#define RTCC_DATE_ASIA			0x02
#define RTCC_DATE_US			0x04
/* time format flags */
#define RTCC_TIME_HMS			0x01
#define RTCC_TIME_HM			0x02

/* square wave contants */
#define SQW_DISABLE     B00000000
#define SQW_32KHZ       B10000000
#define SQW_1024HZ      B10000001
#define SQW_32HZ        B10000010
#define SQW_1HZ         B10000011

/* methods */
uint8_t decToBcd(uint8_t value);
uint8_t bcdToDec(uint8_t value);
/* time variables */
uint8_t hour;
uint8_t minute;
uint8_t sec;
uint8_t day;
uint8_t weekday;
uint8_t month;
uint8_t year;
/* alarm */
uint8_t alarm_hour;
uint8_t alarm_minute;
uint8_t alarm_weekday;
uint8_t alarm_day;
/* support */
uint8_t status1;
uint8_t status2;
uint8_t century;

char strOut[9];
char strDate[11];
/*global error status*/
i2c_err_t err;

/* Private internal functions, but useful to look at if you need a similar func. */
uint8_t decToBcd(uint8_t val)
{
  return ( (val/10*16) + (val%10) );
}

uint8_t bcdToDec(uint8_t val)
{
  return ( (val/16*10) + (val%16) );
}


/* zero out all values, disable all alarms */
uint8_t pcf8563_init(void)		
{
	uint8_t addr=RTCC_ADDR;  /*i2c lib wants the 7bit addr*/
	uint8_t reg=0x0;
	const uint8_t buf[]={0x0,0x0,0x0,0x14,0x17,0x22,0x06,0x12,0x24,0x80,0x80,0x80,0x80,0x0,0x0};
	err=0;

	return i2c_write(addr,reg,buf,15);
	//if (err > 0) { printf("err rtc init: %d\n",err); }
}

/* read 5 bytes and get the 2 status values and the time data */
uint8_t pcf8563_get_time()
{
	uint8_t buf[5]={0,0,0,0,0};
	err=0;
	err = i2c_read(RTCC_ADDR,RTCC_STAT1_ADDR,buf, 5);
	if (err == 0){
		status1=buf[0];
		status2=buf[1];
		//0x7f = 0b01111111
		sec=bcdToDec(buf[2] & 0x7f);
		minute = bcdToDec(buf[3] & 0x7f);
		//0x3f = 0b00111111
		hour = bcdToDec(buf[4] & 0x3f);
        return 0;
	}
	else { 
        return err; 
    }
}

// void pcf8563_print_time()
// {
// 	printf("%d:%d:%d\n", hour, minute, sec);
// }

/* read 4 bytes and translate the date values */
void pcf8563_get_date()
{
	uint8_t buf[4]={0,0,0,0};
	err=0;
	err = i2c_read(RTCC_ADDR,RTCC_DAY_ADDR,buf, 4);
	 //0x3f = 0b00111111
    day = bcdToDec(buf[0] & 0x3f);
    //0x07 = 0b00000111
    weekday = bcdToDec(buf[1] & 0x07);
    //get raw month data byte and set month and century with it.
    month = buf[2];
    if (month & RTCC_CENTURY_MASK) {
        century = 1;
    }
    else {
        century = 0;
    }
    //0x1f = 0b00011111
    month = month & 0x1f;
    month = bcdToDec(month);
	year = bcdToDec(buf[3]);
}

// void pcf8563_print_date()
// {
// 	printf(" %d:%d:%d\n", month, day, year);
// }


char *pcf8563_format_time(uint8_t style)
{
    pcf8563_get_time();
    switch (style) {
        case RTCC_TIME_HM:
            strOut[0] = '0' + (hour / 10);
            strOut[1] = '0' + (hour % 10);
            strOut[2] = ':';
            strOut[3] = '0' + (minute / 10);
            strOut[4] = '0' + (minute % 10);
            strOut[5] = '\0';
            break;
        case RTCC_TIME_HMS:
        default:
            strOut[0] = '0' + (hour / 10);
            strOut[1] = '0' + (hour % 10);
            strOut[2] = ':';
            strOut[3] = '0' + (minute / 10);
            strOut[4] = '0' + (minute % 10);
            strOut[5] = ':';
            strOut[6] = '0' + (sec / 10);
            strOut[7] = '0' + (sec % 10);
            strOut[8] = '\0';
            break;
        }
    return strOut;
}

char *pcf8563_format_date(uint8_t style)
{
    pcf8563_get_date();

        switch (style) {

        case RTCC_DATE_ASIA:
            //do the asian style, yyyy-mm-dd
            if ( century == 1 ){
                strDate[0] = '1';
                strDate[1] = '9';
            }
            else {
                strDate[0] = '2';
                strDate[1] = '0';
            }
            strDate[2] = '0' + (year / 10 );
            strDate[3] = '0' + (year % 10);
            strDate[4] = '-';
            strDate[5] = '0' + (month / 10);
            strDate[6] = '0' + (month % 10);
            strDate[7] = '-';
            strDate[8] = '0' + (day / 10);
            strDate[9] = '0' + (day % 10);
            strDate[10] = '\0';
            break;
        case RTCC_DATE_US:
        //the pitiful US style, mm/dd/yyyy
            strDate[0] = '0' + (month / 10);
            strDate[1] = '0' + (month % 10);
            strDate[2] = '/';
            strDate[3] = '0' + (day / 10);
            strDate[4] = '0' + (day % 10);
            strDate[5] = '/';
            if ( century == 1 ){
                strDate[6] = '1';
                strDate[7] = '9';
            }
            else {
                strDate[6] = '2';
                strDate[7] = '0';
            }
            strDate[8] = '0' + (year / 10 );
            strDate[9] = '0' + (year % 10);
            strDate[10] = '\0';
            break;
        case RTCC_DATE_WORLD:
        default:
            //do the world style, dd-mm-yyyy
            strDate[0] = '0' + (day / 10);
            strDate[1] = '0' + (day % 10);
            strDate[2] = '-';
            strDate[3] = '0' + (month / 10);
            strDate[4] = '0' + (month % 10);
            strDate[5] = '-';

            if ( century == 1 ){
                strDate[6] = '1';
                strDate[7] = '9';
            }
            else {
                strDate[6] = '2';
                strDate[7] = '0';
            }
            strDate[8] = '0' + (year / 10 );
            strDate[9] = '0' + (year % 10);
            strDate[10] = '\0';
            break;

    }
    return strDate;
}

/* enable alarm interrupt
 * whenever the clock matches these values an int will
 * be sent out pin 3 of the Pcf8563 chip
 * returns:  err code
 */
uint8_t pcf8563_enable_alarm()
{

    //set status2 AF val to zero
    status2 &= ~RTCC_ALARM_AF;
    //enable the interrupt
    status2 |= RTCC_ALARM_AIE;

    //enable the interrupt
    //Wire.beginTransmission(Rtcc_Addr);  // Issue I2C start signal
    //Wire.write((byte)RTCC_STAT2_ADDR);
    //Wire.write((byte)status2);
    //Wire.endTransmission();

    const uint8_t buf[]={status2};
    return i2c_write(RTCC_ADDR,RTCC_STAT2_ADDR,buf,1);
	//if (err > 0) { printf("err rtc enable alarm: %d\n",err); }
}

uint8_t pcf8563_alarm_enabled()
{
    // the dtatus2 byte is updated at each get_time read
    return status2 & RTCC_ALARM_AIE;
}

#endif
