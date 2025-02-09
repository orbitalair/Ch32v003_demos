
# Clock Calendar using NXP RTC PCF8563 chip and CH32v003.

A library file for the PCF8563.  This h file library was
taken from my Arduino class file design.  I simplified 
a lot of the functions in order to fit into the 16kb 
flash.

I also wanted to be able to add other features like touch buttons.  

Pcf8563 Datasheet:  https://www.nxp.com/docs/en/data-sheet/PCF8563.pdf  

# Features

1. Single header .h file for the PCF8563 Real Time Clock.
2. Example of how to add your own files, and mod the ch32v003fun
support mk file.  
3. Example of how to use capacitive touch buttons to program the clock.

# Additional libraries
These already included in these demos.  
Add to your projects.  

1.  lib_i2c from here:  https://github.com/ADBeta/CH32V000x-lib_i2c  
Simply copy the .h and .c file to the project ./lib folder.     
2. The ssd1306_i2c.h and ssd1306.h files.  Modified the ssd1306_i2c.h 
to use the lib_i2c and not initialise the i2c bus again.  font_8x8.h is copied over also.
3. ch32v003_touch.h  
The touch header is from the adc touch project to this location.  

