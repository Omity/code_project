CONFIG_MODULE_SIG = n

KERN_DIR ?= /usr/src/linux-headers-$(shell uname -r)
PWD := $(shell pwd)
obj-m := hello_world.o
all:
	
	make -C $(KERN_DIR) M=$(PWD) modules
	
.PHONY: clean

clean:
	make -C $(KERN_DIR) M=$(PWD) clean

