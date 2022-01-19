### second check , after first parse, then KERNELRELEASE should not be NULL.
ifneq ($(KERNELRELEASE),)

$(warning Second - KERNELRELEASE = $(KERNELRELEASE))

### MUST named a new KO name, and the KO name should not as same as the C file name!
### Or, Exclude the same obj file from the *-objs list !!!
# KO_NAME_usbtmc_dev := usbtmc_dev_ko
# obj-m := $(KO_NAME_usbtmc_dev).o
# $(KO_NAME_usbtmc_dev)-objs += usbtmc_dev.o

obj-m += spi.o
obj-m += spi2k7_gpio.o


else ### first check : this time KERNELRELEASE is not set yet ...

$(warning First - KERNELRELEASE = $(KERNELRELEASE))

the_project_path := /home/sj03955/auklet
kernel_output := $(the_project_path)/kernel
KERNELDIR := $(the_project_path)/kernel
bin_path := $(the_project_path)/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin

CFLAG   =  -std=c99 -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D_LARGE_FILE_SOURCE
LDFLAG  =  -lrt  -static   -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D_LARGE_FILE_SOURCE
CC      = ${bin_path}/aarch64-linux-gnu-gcc
STRIP   = ${bin_path}/aarch64-linux-gnu-strip

COMMON_MAKEENV :=                                                 \
		M=$(PWD)                                                  \
		O=$(kernel_output)                                        \
		ARCH=arm64                                                \
		CROSS_COMPILE=$(bin_path)/aarch64-linux-gnu-              \

# all:ko clean
# all:ko rename
all: ko spi_config

ko:
	make -j32 -C $(KERNELDIR) $(COMMON_MAKEENV) modules

spi_config:
	$(CC) -Wall -g $(CFLAG) $(LDFLAG) spi_test.c -o spi_config
	$(CC) -Wall -g $(CFLAG) $(LDFLAG) spi2k7.c -o spi2k7
	$(STRIP) spi_config
	$(STRIP) spi2k7

clean:
	make -j32 -C $(KERNELDIR) $(COMMON_MAKEENV) clean
	rm -f spi_config spi2k7

endif
