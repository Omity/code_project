
KERNELS = $(shell uname -r)

obj-m := cdev_driver.o


all: cdev_driver

cdev_driver:
	make -C /lib/modules/$(KERNELS)/build M=$(CURDIR) modules

clean:
	make -C /lib/modules/$(KERNELS)/build M=$(CURDIR) clean
