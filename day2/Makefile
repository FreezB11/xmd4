ifneq ($(KERNELRELEASE),)
    obj-m := ldd2.o
else
    KERN_DIR ?= /lib/modules/$(shell uname -r)/build/
    PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) clean
	rm -f *.o *.mod.c *.mod *.symvers *.order
endif
