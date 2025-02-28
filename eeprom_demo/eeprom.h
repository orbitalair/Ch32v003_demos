/*****
 *  NAME
 *    24LC eeprom support routines for ch32vv003fun
 *  AUTHOR
 *    Joe Robertson, jmr
 *    orbitalair@gmail.com
 *  CREATION DATE
 *    12/28/2024,  init - built off of arduino rtc code
 *  NOTES
 *    PCF8563 Datasheet: https://www.nxp.com/docs/en/data-sheet/PCF8563.pdf
 *  HISTORY
 *  TODO
 *    Add timer routines
 ******
 *  Robodoc embedded documentation.
 *  http://www.xs4all.nl/~rfsber/Robo/robodoc.html
 */

#ifndef Eeprom_H
#define Eeprom_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lib_i2c.h"

/* The 7 bit addr for the eeprom 
   The i2c lib auto adds the last 0 or 1 bit for read or write */
#define I2C_ADDR    0x52  
#define EEP_PGSZ    64  /* page size in bytes */


/* eeprom usually has a page size and pages start at 0x0.
they will write page size bytes, but if not started at a
page boundary different things may happen.  data may stop 
writing, or roll over to the beginning of the page. */
uint8_t eep_write(uint16_t addr, uint8_t *buf, uint8_t bufsize)
{
    uint8_t testsize;

    /* check page start addr, if not on a boundary and
       data wont fit then return a err code */
    printf("mod addr: %d\n",addr%EEP_PGSZ);
    if ( addr % EEP_PGSZ != 0 ){
        /* adjust the number of bytes to write */
        testsize = EEP_PGSZ - (addr % EEP_PGSZ);
        if (testsize > bufsize){  /*then this data will fit */
            printf("testsize, adjusted:%d\n",testsize);
        }
        else {  /* the buf data wont fit which may cause strange results */
            return 253;
        }
    }
    /* write the data */
    printf("in page write, 0x%x\n",buf[0]);
    if (sizeof(*buf)<=64) {
        return i2c_write_2ba(I2C_ADDR,addr&0x00ff,addr>>8,buf,bufsize);
    }
    else {
        /* buffer is too large for write area */
        return 255;
    }
}

uint8_t eep_read(uint16_t addr, uint8_t *buf, uint8_t bufsize)
{
    i2c_err_t ret;
    printf("in page read, 0x%x\n",buf[0]);
    printf("size of buf, %d\n",bufsize);
    ret= i2c_read_2ba(I2C_ADDR,addr&0x00ff,addr>>8,buf,bufsize);
    printf("retval=%d\n", ret);
    return ret;
}



#endif
