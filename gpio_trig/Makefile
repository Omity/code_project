### second check , after first parse, then KERNELRELEASE should not be NULL.
CC=/home/GJ/Desktop/Tina_Linux/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-gcc-6.4.1
ifneq ($(KERNELRELEASE),)

$(warning Second - KERNELRELEASE = $(KERNELRELEASE))

### MUST named a new KO name, and the KO name should not as same as the C file name!
### Or, Exclude the same obj file from the *-objs list !!!
# KO_NAME_usbtmc_dev := usbtmc_dev_ko
# obj-m := $(KO_NAME_usbtmc_dev).o
# $(KO_NAME_usbtmc_dev)-objs += usbtmc_dev.o

obj-m := gpio_trig.o



else ### first check : this time KERNELRELEASE is not set yet ...

$(warning First - KERNELRELEASE = $(KERNELRELEASE))

the_project_path := /home/GJ/Desktop/sousa_os
kernel_output := $(the_project_path)/lichee/linux-3.10
KERNELDIR := $(the_project_path)/lichee/linux-3.10
bin_path := $(the_project_path)/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/

COMMON_MAKEENV :=                                                \
		M=$(PWD)                                                 \
		O=$(kernel_output)                                       \
		ARCH=arm                                                 \
		CROSS_COMPILE=$(bin_path)/arm-openwrt-linux-muslgnueabi- \

# all:ko clean
# all:ko rename
all: ko gpio_test

ko:
	make -j32 -C $(KERNELDIR) $(COMMON_MAKEENV) modules

gpio_test:
	$(CC) -Wall -g gpio_trig_test.c -o gpio_test

clean:
	make -j32 -C $(KERNELDIR) $(COMMON_MAKEENV) clean
	rm -f gpio_test

endif
