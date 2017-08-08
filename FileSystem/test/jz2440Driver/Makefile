KERN_DIR = /work/cd0/system/linux-3.4.2

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

#obj-m	 += firstDriver.o
#obj-m   += globalmem.o
obj-m    += globalfifo.o



