#ifndef _USART_H
#define _USART_H

#include "stm32f10x.h"  
#include "stdio.h"
#include "stdarg.h"	 
#include "string.h"
#include "lora_cfg.h"

#define USART_REC_LEN 10
#define USART3_MAX_RECV_LEN		1024			    //最大接收缓存字节数
#define USART3_MAX_SEND_LEN		600					//最大发送缓存字节数

extern u8 Lora_mode;
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern vu16 USART3_RX_STA;   						//接收数据状态
extern u8 USART_RX_BUF[USART_REC_LEN];

struct uartParam{
	// 串口名字
	USART_TypeDef* usartName;
	// 时钟分组2
	uint32_t RCC_APB2Periph;
	// 时钟分组1
	uint32_t RCC_APB1Periph;
	// GPIO分组
	GPIO_TypeDef* GPIOx;
	// 接收引脚
	uint16_t RxGPIOPin;
	// 发送引脚
	uint16_t TxGPIOPin;
	// 串口配置信息结构体
	USART_InitTypeDef USART_InitStructure;
};

/******************************
* 函数功能：串口初始化
* 函数参数：
*			struct uartParam*usart:串口信息结构体 内含串口引脚分组及引脚波特率等必要信息
*			NVIC_InitTypeDef*NVIC_InitStructure：NIVC信息结构体 内含NVIC通道、抢占优先级等必要信息
* 函数返回值：无
*******************************/
void usartInit(struct uartParam*usart,NVIC_InitTypeDef*NVIC_InitStructure);

/***********************************************
* 函数功能：串口波特率和校验位配置
* 函数参数：
*			struct uartParam*usart：串口信息结构体
*			bps:波特率（1200~115200）
*			parity:校验位（无、偶、奇）
* 函数返回值：无
***********************************************/
void setUsartParam(struct uartParam*usart,u8 bps,u8 parity);

/**********************************************************
* 函数功能：设置串口收发模式
* 函数参数：
*			struct uartParam*usart：传入串口结构体
*			u8 state：传入串口模式 0-收发模式 1-只发送模式 2-只接收模式
* 函数参数：无
**********************************************************/
void setUsartMod(struct uartParam*usart,u8 mod);

void u3_printf(char* fmt,...);

#endif

