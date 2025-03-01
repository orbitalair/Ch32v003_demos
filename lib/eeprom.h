/**
 *  @brief 24LC eeprom support routines for ch32fun
 *  @author Joe Robertson, jmr, orbitalair@gmail.com
 *  @note  PCF8574 Datasheet: https://www.ti.com/lit/ds/symlink/pcf8574.pdf
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


/** 
 * @brief eeprom usually has a page size and pages start at 0x0.
 * they will write page size bytes, but if not started at a
 * page boundary different things may happen.  data may stop 
 * writing, or roll over to the beginning of the page. 
 * @param addr 2 byte address to write the buffer to
 * @param buf  the data to write
 * @param bufsize  size of the data buffer 
 * @return 0 for ok, or 253 for error OR the buf was not aligned properly
 * or regular i2c return codes
 */
uint8_t eep_write(uint16_t addr, uint8_t *buf, uint8_t bufsize)
{
    uint8_t testsize;

    /* check page start addr, if not on a boundary and
       data wont fit then return a err code */
    if ( addr % EEP_PGSZ != 0 ){
        /* adjust the number of bytes to write */
        testsize = EEP_PGSZ - (addr % EEP_PGSZ);
        if (testsize > bufsize){  /*then this data will fit */
        }
        else {  /* the buf data wont fit which may cause strange results */
            return 253;
        }
    }
    /* write the data */
    if (sizeof(*buf)<=64) {
        return i2c_write_2ba(I2C_ADDR,addr&0x00ff,addr>>8,buf,bufsize);
    }
    else {
        /* buffer is too large for write area */
        return 255;
    }
}

/** 
 * @brief eeprom reading is more forgiving. 
 * Here just follow the same 64byte read, but the user can 
 * read as few bytes as desired, and also read from any
 * address location.
 * @param addr 2 byte address to write the buffer to
 * @param buf  the data to write
 * @param bufsize  size of the data buffer 
 * @return regular i2c lib return codes
 */
uint8_t eep_read(uint16_t addr, uint8_t *buf, uint8_t bufsize)
{
    i2c_err_t ret;
    ret= i2c_read_2ba(I2C_ADDR,addr&0x00ff,addr>>8,buf,bufsize);
    return ret;
}

#endif
