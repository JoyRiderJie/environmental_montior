#ifndef _LED_H
#define _LED_H

#include "stm32f10x.h"  

/**********************************
* 函数功能：LED的初始化函数
* 函数参数：无
* 函数返回值：无
***********************************/
void ledInit(void);
	
/**********************************
* 函数功能：LED的初始化函数
* 函数参数：无
* 函数返回值：无
***********************************/
void ledControl(uint16_t location,BitAction state);


#endif
