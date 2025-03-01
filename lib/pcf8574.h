/**
 *  @brief pcf8574 i2c port expander support routines for ch32vv003fun
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

#ifndef Pcf8574_H
#define Pcf8574_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lib_i2c.h"

/* The 7 bit addr for the port expander
   The i2c lib auto adds the last 0 or 1 bit for read or write 
   0x40 for all 3 addr pins grounded
   0x4e for all 3 addr pins high
   */
#define I2C_ADDR    0x38 

/* a pins bitfield */
uint8_t pins = 0;

/** @brief Clear all the pins. */
void pcf8574_clear_all_pins(){
    pins=0x0;
}

/** @brief Set all pins high. */
void pcf8574_set_all_pins(){
    pins=0xff;
}

/** @brief Set a pin to high.
 *  @param uint8_t pin, 0-7
 */
void pcf8574_set_pin( uint8_t pin){
    if ( pin >=0 && pin <=7){
        pins = pins | (1 << pin);
    }
}

/** @brief Set a pin off. 
 *  @param uint8_t pin, 0-7
 */
void pcf8574_clear_pin(uint8_t pin){
    pins = pins & ~(1 << pin);
}

/** @brief Write the pin settings to the device. */
uint8_t pcf8574_write_pins(){
    return i2c_write(I2C_ADDR,0x0,&pins,1);
}

/** @brief Read the pin states from the device. */
uint8_t pcf8574_read_pins(){
    return i2c_read(I2C_ADDR,0x0,&pins,1);
}


#endif
