#include "time.h"
 
/********************************************************************
* 函数功能：通用定时器中断初始化
* 函数参数：
*			struct timeParam*timParam：定时器配置结构体
*			NVIC_InitTypeDef NVIC_InitStructure：定时器NVIC配置结构体
* 函数返回值：无
* 附：定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us. Ft=定时器工作频率,单位:Mhz 	
*********************************************************************/
void timInit(struct timeParam*timParam,NVIC_InitTypeDef*NVIC_InitStructure)
{	
	if(timParam->RCC_APB1Periph)
		RCC_APB1PeriphClockCmd(timParam->RCC_APB1Periph, ENABLE);//TIM3时钟使能    
	
	TIM_ClearITPendingBit(timParam->timeName,TIM_IT_Update);
	TIM_TimeBaseInit(timParam->timeName, &timParam->TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	TIM_ITConfig(timParam->timeName,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断
	TIM_Cmd(timParam->timeName,ENABLE);//开启定时器3
	
	NVIC_Init(NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

/******************** 定时2相关  **********************/
uint16_t time2Count = 0;
//定时器2中断服务程序		    
void TIM2_IRQHandler(void)
{ 		
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		if(++time2Count==65534) time2Count = 0;		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除更新中断标志 
//		TIM_Cmd(TIM2, DISABLE); //关闭TIM3
//		
//		if(time2Count++ == 65535) time2Count = 0;		
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除更新中断标志 
	}	    
}

/******************** 定时器3相关 *********************************/

/************************************************
* 函数功能：配置TIM3预装载周期值
* 函数参数：
*			TIM_TypeDef* timeName：定时器名
* 			u16 period：重新设置的定时器预装载周期
* 函数返回值：无
******************************************************************/
void TIM3_SetARR(TIM_TypeDef* timeName,u16 period)
{
	 TIM_SetCounter(timeName,0);//计数器清空
	 timeName->ARR&=0x00;       //先清预装载周期值为0
	 timeName->ARR|= period;    //更新预装载周期值
}

//定时器3中断服务程序		    
void TIM3_IRQHandler(void)
{ 		
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除TIM3更新中断标志 
		USART3_RX_STA|=1<<15;	 //标记接收完成
		TIM_Cmd(TIM3, DISABLE); //关闭TIM3
	}	    
}

