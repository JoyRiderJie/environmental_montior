#ifndef __CONFIG_H
#define __CONFIG_H


/* STM32 files */
#include "stm32f10x.h"   

/* C files */
#include "string.h"
#include "stdio.h"

/* devices files  */
#include "led.h"
#include "usart.h"
#include "lora_app.h"
#include "lora_ui.h"
#include "delay.h"


/* FreeRTOS files */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* 开发板外设功能 */
#define 	USARTFUNCTION			01

/* 事件组的事件   */
#define 	EVENT1 		(0x01 << 0)
#define 	EVENT2 		(0x01 << 1)
#define 	EVENT3 		(0x01 << 2)
#define 	EVENT4 		(0x01 << 3)
#define 	EVENT5 		(0x01 << 4)
#define 	EVENT6 		(0x01 << 5)
#define	 	EVENT7 		(0x01 << 6)
#define 	EVENT8 		(0x01 << 7)

/*  消息队列的结构体  */
struct queueMsg{
	// 消息的ID
	int id;
	// 消息内容
	void*text;
};

/* NVIC结构体  */
struct nvicParam{
	// NVIC通道
	uint8_t NVIC_IRQChannel;
	// 抢占优先级
	uint8_t NVIC_IRQChannelPreemptionPriority;
	// 响应优先级
	uint8_t NVIC_IRQChannelSubPriority;
};
   
/*******************************************
* 函数功能：系统外设 储存初始化
* 函数参数：无
* 函数返回值：无
*******************************************/
void sysInit(void);

/*******************************************
* 函数功能：系统工作 内部不含while函数
* 函数参数：无
* 函数返回值：无
*******************************************/
void sysWork(void);


#endif



