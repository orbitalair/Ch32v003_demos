all : flash


TARGET:=main
ADDITIONAL_C_FILES:=../lib/lib_i2c.c 
TARGET_MCU?=CH32V003
MINICHLINK?=~/coding/ch32/ch32fun/minichlink/
include ../ch32fun/ch32fun.mk
CFLAGS+=-I../lib

flash : cv_flash
clean : cv_clean


