# 指定需要编译的文件
# obj-y += bmp.o
# obj-y += lcd.o
# obj-y += music.o
# obj-y += event.o

# 首先使用 wildcard 函数获取工作目录下的.c 文件列表；之后将列表中所有文件名的后缀.c 替换为.o
obj-y += $(patsubst %.c,%.o,$(wildcard *.c)) 