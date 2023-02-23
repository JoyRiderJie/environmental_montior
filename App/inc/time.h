#ifndef _TIME_H
#define _TIME_H

#include "stm32f10x.h"  

extern vu16 USART3_RX_STA;

struct timeParam{
	// 定时器名字
	TIM_TypeDef* timeName;
	// 设置时钟
	uint32_t RCC_APB1Periph;
	// 定时器参数设置
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
};

/********************************************************************
* 函数功能：通用定时器中断初始化
* 函数参数：
*			struct timeParam*timParam：定时器配置结构体
*			NVIC_InitTypeDef NVIC_InitStructure：定时器NVIC配置结构体
* 函数返回值：无
* 附：定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us. Ft=定时器工作频率,单位:Mhz 	
*********************************************************************/
void timInit(struct timeParam*timParam,NVIC_InitTypeDef*NVIC_InitStructure);

/************************************************
* 函数功能：配置TIM3预装载周期值
* 函数参数：
*			TIM_TypeDef* timeName：定时器名
* 			u16 period：重新设置的定时器预装载周期
* 函数返回值：无
******************************************************************/
void TIM3_SetARR(TIM_TypeDef* timeName,u16 period);

#endif
