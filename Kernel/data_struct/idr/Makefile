# at first type on ur terminal that $(uname -r) then u will get the version..
# that is using on ur system

obj-m += ${object}.o

KDIR =/usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order
