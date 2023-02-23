#include "time.h"
 
/********************************************************************
* �������ܣ�ͨ�ö�ʱ���жϳ�ʼ��
* ����������
*			struct timeParam*timParam����ʱ�����ýṹ��
*			NVIC_InitTypeDef NVIC_InitStructure����ʱ��NVIC���ýṹ��
* ��������ֵ����
* ������ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us. Ft=��ʱ������Ƶ��,��λ:Mhz 	
*********************************************************************/
void timInit(struct timeParam*timParam,NVIC_InitTypeDef*NVIC_InitStructure)
{	
	if(timParam->RCC_APB1Periph)
		RCC_APB1PeriphClockCmd(timParam->RCC_APB1Periph, ENABLE);//TIM3ʱ��ʹ��    
	
	TIM_ClearITPendingBit(timParam->timeName,TIM_IT_Update);
	TIM_TimeBaseInit(timParam->timeName, &timParam->TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ITConfig(timParam->timeName,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
	TIM_Cmd(timParam->timeName,ENABLE);//������ʱ��3
	
	NVIC_Init(NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

/******************** ��ʱ2���  **********************/
uint16_t time2Count = 0;
//��ʱ��2�жϷ������		    
void TIM2_IRQHandler(void)
{ 		
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		if(++time2Count==65534) time2Count = 0;		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //��������жϱ�־ 
//		TIM_Cmd(TIM2, DISABLE); //�ر�TIM3
//		
//		if(time2Count++ == 65535) time2Count = 0;		
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //��������жϱ�־ 
	}	    
}

/******************** ��ʱ��3��� *********************************/

/************************************************
* �������ܣ�����TIM3Ԥװ������ֵ
* ����������
*			TIM_TypeDef* timeName����ʱ����
* 			u16 period���������õĶ�ʱ��Ԥװ������
* ��������ֵ����
******************************************************************/
void TIM3_SetARR(TIM_TypeDef* timeName,u16 period)
{
	 TIM_SetCounter(timeName,0);//���������
	 timeName->ARR&=0x00;       //����Ԥװ������ֵΪ0
	 timeName->ARR|= period;    //����Ԥװ������ֵ
}

//��ʱ��3�жϷ������		    
void TIM3_IRQHandler(void)
{ 		
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //���TIM3�����жϱ�־ 
		USART3_RX_STA|=1<<15;	 //��ǽ������
		TIM_Cmd(TIM3, DISABLE); //�ر�TIM3
	}	    
}

