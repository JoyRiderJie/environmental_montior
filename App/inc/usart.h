#ifndef _USART_H
#define _USART_H

#include "stm32f10x.h"  
#include "stdio.h"
#include "stdarg.h"	 
#include "string.h"
#include "lora_cfg.h"

/*********************************     LORA 串口3相关      **************************/
#define USART_REC_LEN 10
#define USART3_MAX_RECV_LEN		1024			    //最大接收缓存字节数
#define USART3_MAX_SEND_LEN		600					//最大发送缓存字节数

extern u8 Lora_mode;
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern vu16 USART3_RX_STA;   						//接收数据状态
extern u8 USART_RX_BUF[USART_REC_LEN];


/*********************************     ESP8266 串口4相关      **************************/
#define UART4_REC_LEN					255	//定义最大接收字节数  开始是255
#define EN_UART4_RX					1	//串口接收使能。
											//1：允许接收
											//0：禁止接收

/******************************************************************************
                              外部调用功能函数
******************************************************************************/
extern uint8_t  UART4_RX_BUF[UART4_REC_LEN];	//接收缓冲，最大UART4_REC_LEN个字节，末字节为换行符 
extern uint16_t UART4_RX_COUNT;				//UART4接收到数据个数
extern uint8_t F_UART4_RX_FINISH;				//接收状态标记,1:UART4接收完成，0：UART4接收未完成
extern uint8_t F_UART4_RX_RECEIVING;			//串口接收正在进行中标记,1:进行中，0：暂停或结束
extern uint8_t UART4_RX_TIMEOUT_COUNT;			//串口接收超时计数。串口超过3ms未接收到数据，视为超时。单位：ms

/***************   通用结构体  **********************************************/
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

void UART4_SendData(uint16_t Data);
void UART4_SendStr(uint8_t *Data);
void UART4_SendStrlen(uint8_t *Data, uint8_t len);



#endif

