#ifndef __DELAY_H
#define __DELAY_H 	

#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ʹ��SysTick����ͨ����ģʽ���ӳٽ��й����ʺ�STM32F10xϵ�У�
//����delay_us,delay_ms
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2010/1/1
//�汾��V1.8
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
////////////////////////////////////////////////////////////////////////////////// 
	 
	 
extern uint16_t time2Count;

// ���ʹ�õ���ʱ����
//void delay_init(void);
//void delay_ms(u16 nms);
//void delay_us(u32 nus);

//// ʹ�ö�ʱ��2��ɵ���ʱ����
//void Tim2Delay_us(__IO uint32_t nTime);

uint32_t DWT_Dalay_Init(void);
//��ʱ����,������n΢��
void DWT_Delay_us(volatile uint32_t nus);

#endif





























