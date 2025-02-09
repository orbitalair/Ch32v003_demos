all : flash


TARGET:=main
ADDITIONAL_C_FILES:=eeprom.h lib_i2c.c 
TARGET_MCU?=CH32V003
MINICHLINK?=~/coding/ch32vxxx/ch32v003fun/minichlink/
include ../ch32fun/ch32fun.mk


flash : cv_flash
clean : cv_clean


