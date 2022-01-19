CONFIG_MODULE_SIG=n


ifneq ($(KERNELRELEASE),)

obj-m       += misc_driver.o


else
# generate the path
CURRENT_PATH      := $(shell pwd)
LINUX_KERNEL_PATH := /lib/modules/$(shell uname -r)/build

CFLAG   =  -std=c99 -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D_LARGE_FILE_SOURCE
LDFLAG  =  -lrt  -static   -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D_LARGE_FILE_SOURCE

FILE_BIN := misc_driver_test.c
TARGET   := misc_test

all: ko $(TARGET)

ko:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
$(TARGET):
	gcc -Wall -g $(CFLAG) $(LDFLAG) $(FILE_BIN) -o $(TARGET)
	strip $(TARGET)
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
	rm -f $(TARGET)
endif
