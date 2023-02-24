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

/* ���������蹦�� */
#define 	USARTFUNCTION			01

/* �¼�����¼�   */
#define 	EVENT1 		(0x01 << 0)
#define 	EVENT2 		(0x01 << 1)
#define 	EVENT3 		(0x01 << 2)
#define 	EVENT4 		(0x01 << 3)
#define 	EVENT5 		(0x01 << 4)
#define 	EVENT6 		(0x01 << 5)
#define	 	EVENT7 		(0x01 << 6)
#define 	EVENT8 		(0x01 << 7)

/*  ��Ϣ���еĽṹ��  */
struct queueMsg{
	// ��Ϣ��ID
	int id;
	// ��Ϣ����
	void*text;
};

/* NVIC�ṹ��  */
struct nvicParam{
	// NVICͨ��
	uint8_t NVIC_IRQChannel;
	// ��ռ���ȼ�
	uint8_t NVIC_IRQChannelPreemptionPriority;
	// ��Ӧ���ȼ�
	uint8_t NVIC_IRQChannelSubPriority;
};
   
/*******************************************
* �������ܣ�ϵͳ���� �����ʼ��
* ������������
* ��������ֵ����
*******************************************/
void sysInit(void);

/*******************************************
* �������ܣ�ϵͳ���� �ڲ�����while����
* ������������
* ��������ֵ����
*******************************************/
void sysWork(void);


#endif



