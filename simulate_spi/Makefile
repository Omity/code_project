ifneq($(KERNELRELEASE),)
	obj-m :=spi_test.o
else
CURRENT_PATH :=$(shell pwd)
LINUX_KERNEL_PATH :=/home/sj03955/auklet/build

default:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean

endif
