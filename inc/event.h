#ifndef _EVENT_H
#define _EVENT_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "config.h"

/*
 *  get_inputevent: 用来获取触摸屏输入事件
 *  arg：
 *      @int*getlocation:保存触摸屏的始末位置
 *  return：
 *      成功返回0
 *      失败返回-1
**/
int get_inputevent(int*getlocation,int fd);


#endif 

