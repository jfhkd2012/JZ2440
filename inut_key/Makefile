KERN_DIR := /home/jf/Program/JZ2440/linux/linux-2.6.22.6
all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= input_key_drv.o
