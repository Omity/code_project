CONFIG_MODULE_SIG = n
all:
#ifneq ($(KERNNELRELASE),)
	obj-m := hello_world.o
	
#else
	KERN_DIR ?= /usr/src/linux-headers-$(shell uname -r)
	PWD := $(shell pwd)
	
#default:
	make -C $(KERN_DIR) M=$(PWD) modules
	
#endif 
.PHONY: clean

clean:
	make -C $(KERN_DIR) M=$(PWD) clean

