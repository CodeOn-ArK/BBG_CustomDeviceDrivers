obj-m := pcd.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERNEL_DIR=/home/ark/linux_workspace/embedded_linux_workspace/ldd/source/linux
HOST_KERNEL_DIR=/lib/modules/$(shell uname -r)/build/

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) clean
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) help
host:
	make -C $(HOST_KERNEL_DIR) M=$(PWD) modules
transfer:
	scp *.c debian@192.168.7.2:modules                                                           
