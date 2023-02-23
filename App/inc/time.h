#ifndef _TIME_H
#define _TIME_H

#include "stm32f10x.h"  

extern vu16 USART3_RX_STA;

struct timeParam{
	// ��ʱ������
	TIM_TypeDef* timeName;
	// ����ʱ��
	uint32_t RCC_APB1Periph;
	// ��ʱ����������
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
};

/********************************************************************
* �������ܣ�ͨ�ö�ʱ���жϳ�ʼ��
* ����������
*			struct timeParam*timParam����ʱ�����ýṹ��
*			NVIC_InitTypeDef NVIC_InitStructure����ʱ��NVIC���ýṹ��
* ��������ֵ����
* ������ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us. Ft=��ʱ������Ƶ��,��λ:Mhz 	
*********************************************************************/
void timInit(struct timeParam*timParam,NVIC_InitTypeDef*NVIC_InitStructure);

/************************************************
* �������ܣ�����TIM3Ԥװ������ֵ
* ����������
*			TIM_TypeDef* timeName����ʱ����
* 			u16 period���������õĶ�ʱ��Ԥװ������
* ��������ֵ����
******************************************************************/
void TIM3_SetARR(TIM_TypeDef* timeName,u16 period);

#endif
