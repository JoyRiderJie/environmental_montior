# target:main.c ./src/lib.a
# 	arm-linux-gcc -o target main.c ./src/lib.a -I ./inc

# ./src/lib.a:./src/bmp.o ./src/event.o ./src/lcd.o ./src/music.o
# 	ar crs ./src/lib.a ./src/bmp.o ./src/event.o ./src/lcd.o ./src/music.o 

# ./src/bmp.o:./src/bmp.c
# 	arm-linux-gcc ./src/bmp.c -c -o ./src/bmp.o -I ./inc

# ./src/event.o:./src/event.c
# 	arm-linux-gcc ./src/event.c -c -o ./src/event.o -I ./inc

# ./src/lcd.o:./src/lcd.c
# 	arm-linux-gcc ./src/lcd.c -c -o ./src/lcd.o -I ./inc

# ./src/music.o:./src/music.c
# 	arm-linux-gcc ./src/music.c -c -o ./src/music.o -I ./inc

# 指定交叉编译器
CROSS_COMPILE = arm-linux-
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

# 定义编译参数  这里指定头文件库
CFLAGS := -Wall -O2 -g
CFLAGS += -I $(shell pwd)/inc

# 定义链接参数
LDFLAGS := 

# 说明顶层的编译参数与链接参数可以被其他的make进程继承
export CFLAGS LDFLAGS

# 获取当前目录
TOPDIR := $(shell pwd)
# 将当前目录传递给其他的目录
export TOPDIR

# 指定make最终生成的文件
TARGET := target

# 添加当前目录中需要编译的文件
obj-y += main.o
# 添加二级目录中需要编译的文件 这里需要注意的是 / 符号不能够省略
obj-y += src/

# make的起始位置
all : start_recursive_build $(TARGET)
	@echo $(TARGET) has been built!

# 转而执行makefile.build文件
start_recursive_build:
	make -C ./ -f $(TOPDIR)/Makefile.build

# 生成需要的目标文件
$(TARGET) : built-in.o
	$(CC) -o $(TARGET) built-in.o $(LDFLAGS)

clean:
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	