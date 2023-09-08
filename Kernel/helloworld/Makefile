target = hello
obj-m := $(target).o
KERNELDIR = /usr/src/linux-headers-$(shell uname -r)
default:
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules
install:
	insmod $(target).ko
uninstall:
	rmmod $(target).ko
clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order
