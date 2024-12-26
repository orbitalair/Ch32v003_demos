
# Clock Calendar using NXP RTC PCF8563 chip and CH32v003.

A library file for the PCF8563.  This h file library was
taken from my Arduino class file design.  I simplified 
a lot of the functions in order to fit into the 16kb 
flash to be able to add other features like touch buttons.  


# Features

1. Single header .h file for the PCF8563 Real Time Clock.
2. Example of how to add your own files, and mod the ch32v003fun
support mk file.  My system did not like the riscv-linux toolchain. I had to use the riscv-unknown toolchain.
3. Example of how to use capacitive touch buttons to program the clock.

# Additional libraries

1.  lib_i2c from here:  
Simply copy the .h and .c file to the project folder.  
2. the ssd1306_i2c.h and ssd1306.h files.  I have modified the ssd1306_i2c.h 
to use the lib_i2c and not initialise the i2c bus again.
3. ch32v003_touch.h  
I copied the touch header from the adc touch project to this location.  

