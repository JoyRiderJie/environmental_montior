#include "MQ2.h"

void ADC_Pin_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
						| RCC_APB2Periph_ADC1,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;//单次转换
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//数据对齐
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_InitStruct.ADC_NbrOfChannel = 1;//转换总数
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;//单通道扫描
	ADC_Init(ADC1,&ADC_InitStruct);
	
	//转换通道 第几次转换 采样时间
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
	
	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
	
	ADC_Cmd(ADC1,ENABLE);
}

u16 ADC_Trans(void)
{
	u16 adc_value = 0;
	u8 i = 0;
	
	for(i = 0; i < 50; i++)
	{ 
		//开始转换
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
		
		//转换是否结束
		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) != SET);
		adc_value = adc_value + ADC_GetConversionValue(ADC1);//读ADC中的值
	}
	
	return adc_value / 50;//取样50次的平均值
}

