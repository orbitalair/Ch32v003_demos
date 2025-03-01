/**
 *  @brief PCF8563 Real Time Clock support routines for ch32vv003fun
 *  @author Joe Robertson, jmr, orbitalair@gmail.com
 *  @note  PCF8563 Datasheet: https://www.nxp.com/docs/en/data-sheet/PCF8563.pdf
 */

 /* 
 * Released under the MIT Licence
 * Copyright ADBeta (c) 2024 - 2025
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef Rtc_Pcf8563_H
#define Rtc_Pcf8563_H

#include <stdio.h>
#include <stdint.h>
#include "lib_i2c.h"

/* The 7 bit addr for pcf8563 rtc 
   The i2c lib auto adds the last 0 or 1 bit for read or write */
#define RTCC_ADDR    0x51  

/* register addresses in the rtc */
#define RTCC_STAT1_ADDR			0x0
#define RTCC_STAT2_ADDR			0x01
#define RTCC_DAY_ADDR 			0x05
#define RTCC_ALRM_MIN_ADDR 	    0x09
#define RTCC_SQW_ADDR 	        0x0D

/* Setting the alarm flag to 0 enables the alarm.
   Set it to 1 to disable the alarm for that value. */
#define RTCC_ALARM				0x80
#define RTCC_ALARM_AIE 			0x02
#define RTCC_ALARM_AF 			0x08 // 0x08 : not 0x04!!!!
/* Optional val for no alarm setting */
#define RTCC_NO_ALARM			99

#define RTCC_CENTURY_MASK 		0x80

/* date format flags */
#define RTCC_DATE_WORLD			0x01
#define RTCC_DATE_ASIA			0x02
#define RTCC_DATE_US			0x04
/* time format flags */
#define RTCC_TIME_HMS			0x01
#define RTCC_TIME_HM			0x02

/* square wave contants, CLK_OUT */
#define SQW_DISABLE     0b00000000
#define SQW_32KHZ       0b10000000
#define SQW_1024HZ      0b10000001
#define SQW_32HZ        0b10000010
#define SQW_1HZ         0b10000011

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
/* to output a formatted string */
char strOut[9];
char strDate[11];
/* global error status from i2c lib */
i2c_err_t err;

/* Pcf8563 uses binary coded decimal for the registers
   see the datasheet  */

/* Decimal to Binary coded decimal */   
uint8_t decToBcd(uint8_t val)
{
  return ( (val/10*16) + (val%10) );
}

/* Binary coded decimal to decimal */
uint8_t bcdToDec(uint8_t val)
{
  return ( (val/16*10) + (val%16) );
}

/* Check a range, if ok return 1, else return 0 */
uint8_t check(uint8_t value, uint8_t min, uint8_t max)
{
    return (value >=min && value <=max);
}


/* Zero out all values, disable all alarms. 
   Requires that the I2C bus is enabled.
   returns i2c_err */
uint8_t pcf8563_init(void)		
{
    /* buffer legend */
    /* status1, status2, secs, min, hr, day, weekday, month, year, alm_min, alm_hr, alm_day, alm_wkday, clkout, timer_ctl, timer*/
	const uint8_t buf[]={0x0,0x0,0x0,0x0,0x12,0x01,0x06,0x01,0x25,0x80,0x80,0x80,0x80,0x0,0x0};
	return i2c_write(RTCC_ADDR,RTCC_STAT1_ADDR,buf,15);
}

/* Clear both status registers */
uint8_t pcf8563_clear_status()
{
    uint8_t buf[2]={0,0};
    return i2c_read(RTCC_ADDR,RTCC_STAT1_ADDR,buf, 2);
}

/* Print the 2 status bytes values */
void pcf8563_format_status()
{
    printf("Status1: 0x%02x, Status2: 0x%02x\n",status1, status2);
}

/* Read 5 bytes and get the 2 status values and the time data */
uint8_t pcf8563_get_time()
{
	uint8_t buf[5]={0,0,0,0,0};
	err=0;
	err = i2c_read(RTCC_ADDR,RTCC_STAT1_ADDR,buf, 5);
	if (err == 0){
		status1=buf[0];
		status2=buf[1];
		/* 0x7f = 0b01111111 */
		sec=bcdToDec(buf[2] & 0x7f);
		minute = bcdToDec(buf[3] & 0x7f);
		/* 0x3f = 0b00111111 */
		hour = bcdToDec(buf[4] & 0x3f);
        return 0;
	}
    return err; 
}

/* Read 4 bytes and translate the date values */
void pcf8563_get_date()
{
	uint8_t buf[4]={0,0,0,0};
	err=0;
	err = i2c_read(RTCC_ADDR,RTCC_DAY_ADDR,buf, 4);
	 /* 0x3f = 0b00111111 */
    day = bcdToDec(buf[0] & 0x3f);
    /* 0x07 = 0b00000111 */
    weekday = bcdToDec(buf[1] & 0x07);
    /* get raw month data byte and set month and century with it. */
    month = buf[2];
    if (month & RTCC_CENTURY_MASK) {
        century = 1;
    }
    else {
        century = 0;
    }
    /* 0x1f = 0b00011111 */
    month = month & 0x1f;
    month = bcdToDec(month);
	year = bcdToDec(buf[3]);
}

/* Print the time in a format style, HM or HMS */
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

/* Print the data in a format style, Asia, USA, and World */
char *pcf8563_format_date(uint8_t style)
{
    pcf8563_get_date();

        switch (style) {

        case RTCC_DATE_ASIA:
            /* do the asian style, yyyy-mm-dd */
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
            /* the US style, mm/dd/yyyy */
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
            /* do the world style, dd-mm-yyyy */
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

/* Set the square wave pin output
   use SQW_DISABLE to disable the output
   returns i2c_err or -1 if input out of bounds. */
uint8_t pcf8563_set_squarewave(uint8_t frequency)
{
    /* verify the input value */
    if (frequency == SQW_1024HZ || frequency == SQW_1HZ || 
        frequency == SQW_32HZ || frequency == SQW_32KHZ ||
        frequency == SQW_DISABLE)
    {
        const uint8_t buf[1]={frequency};
        return i2c_write(RTCC_ADDR,RTCC_SQW_ADDR,buf,1);
    }
    return -1;
}

/* Enable alarm interrupt
 * Whenever the clock matches these values an int will
 * be sent out pin 3 of the Pcf8563 chip
 * returns:  err code, 0=ok
 */
uint8_t pcf8563_enable_alarm()
{
    //set status2 AF val to zero
    status2 &= ~RTCC_ALARM_AF;
    //enable the interrupt
    status2 |= RTCC_ALARM_AIE;

    const uint8_t buf[]={status2};
    return i2c_write(RTCC_ADDR,RTCC_STAT2_ADDR,buf,1);
}

/* Check if the alarm is enabled
   returns 1 if enabled, 0 disabled */
uint8_t pcf8563_alarm_enabled()
{
    /* the status2 byte is updated at each get_time read */
    return status2 & RTCC_ALARM_AIE;
}

/* Returns true if AF is on */
uint8_t pcf8563_alarm_active()
{
    return status2 & RTCC_ALARM_AF;
}


/* General function to set or clear the alarm bit
   Set to out of bounds value to clear the alarm (or RTCC_NO_ALARM)*/
uint8_t set_alarm_bit(uint8_t value, uint8_t min, uint8_t max)
{
    if (check(value, min, max))
    {
        value = decToBcd(value);
        value &= ~RTCC_ALARM;
    }
    else {
        value = 0x0; 
        value |= RTCC_ALARM;
    }
    return value;
}


/* Set the alarm values
 * Whenever the clock matches these values an int will
 * be sent out pin 3 of the Pcf8563 chip
 */
uint8_t pcf8563_set_alarm(uint8_t alarm_minute, uint8_t alarm_hour, uint8_t alarm_day, uint8_t alarm_weekday)
{
    err=0;
    alarm_minute = set_alarm_bit(alarm_minute, 0, 59);
    alarm_hour = set_alarm_bit(alarm_hour, 0, 23);
    alarm_day = set_alarm_bit(alarm_day, 1, 31);
    alarm_weekday = set_alarm_bit(alarm_weekday, 0, 6);

    if ( (err=pcf8563_enable_alarm())==0){
        const uint8_t buf[4]={alarm_minute,alarm_hour,alarm_day,alarm_weekday};
        err= i2c_write(RTCC_ADDR,RTCC_ALRM_MIN_ADDR,buf,4);
    }
    return err;

}

/* Get alarm, set values to RTCC_NO_ALARM (99) if alarm flag is not set */
uint8_t pcf8563_get_alarm()
{
    uint8_t buf[4]={0,0,0,0};
	err=0;
	err = i2c_read(RTCC_ADDR,RTCC_ALRM_MIN_ADDR,buf, 4);
	if (err == 0){
        if(0b10000000 & buf[0]){
            alarm_minute = RTCC_NO_ALARM;
        } else {
            alarm_minute = bcdToDec(buf[0] & 0b01111111);
        }
        if(0b10000000 & buf[1]){
            alarm_hour = RTCC_NO_ALARM;
        } else {
            alarm_hour = bcdToDec(buf[1] & 0b00111111);
        }
        if(0b10000000 & buf[2]){
            alarm_day = RTCC_NO_ALARM;
        } else {
            alarm_day = bcdToDec(buf[2]  & 0b00111111);
        }
        if(0b10000000 & buf[3]){
            alarm_weekday = RTCC_NO_ALARM;
        } else {
            alarm_weekday = bcdToDec(buf[3]  & 0b00000111);
        }
    }
    return err; 
}

/* Print the alarm data bytes */
void pcf8563_format_alarm()
{
    printf("Alarm: %02d:%02d day:%d  weekday:%d\n",alarm_hour,alarm_minute,alarm_day,alarm_weekday);
}

/* Reset the alarm leaving interrupt unchanged */
uint8_t pcf8563_reset_alarm()
{
    /* set status2 AF val to zero to reset alarm */
    status2 &= ~RTCC_ALARM_AF;
    const uint8_t buf[1]={status2};
    return i2c_write(RTCC_ADDR,RTCC_STAT2_ADDR,buf,1);
}

/* Clear the alarm and interrupt settings */
uint8_t pcf8563_clear_alarm()
{
    /* set status2 AF val to zero to reset alarm */
    status2 &= ~RTCC_ALARM_AF;
    /* turn off the interrupt */
    status2 &= ~RTCC_ALARM_AIE;

    const uint8_t buf[1]={status2};
    return i2c_write(RTCC_ADDR,RTCC_STAT2_ADDR,buf,1);
}
#endif
