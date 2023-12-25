#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/input.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <wait.h>

#include "lcd.h"
#include "bmp.h"
#include "event.h"
#include "stdbool.h"

//触摸屏位置结构体
struct location
{
    //起始位置   横坐标
    int start_x;
    //起始位置   纵坐标
    int start_y;
    //终止位置   横坐标
    int end_x;
    //终止位置   纵坐标
    int end_y;
};


/********************************************
*  函数功能:信号量服务函数
*  函数参数:无
*  函数返回值:无
********************************************/
void sysInit(void);

/********************************************
*  函数功能:信号量服务函数
*  函数参数:无
*  函数返回值:无
********************************************/
void sysWork(void);

/********************************************
*  函数功能:父进程函数
*  函数参数:无
*  函数返回值:无
********************************************/
void fatherWork(void);

/********************************************
*  函数功能:子进程函数
*  函数参数:无
*  函数返回值:无
********************************************/
void sonWork(void);

/********************************************
*  函数功能:信号量服务函数
*  函数参数:
*           int sing:进程号
*  函数返回值:无
********************************************/
void my_sig_handler(int sing);


#endif 

 