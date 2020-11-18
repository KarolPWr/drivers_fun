obj-m += device_mod.o
all:
	make -C /lib/modules/5.4.72+/build M=$(PWD) ARCH=arm modules
clean:
	make -C /lib/modules/5.4.72+/build M=$(PWD) ARCH=arm clean
